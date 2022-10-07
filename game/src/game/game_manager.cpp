#include "game/game_manager.h"

#include <chrono>
#include <imgui.h>

#include <fmt/format.h>

#include "maths/basic.h"

#include "utils/conversion.h"
#include "utils/log.h"


#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#endif

namespace game
{
GameManager::GameManager()
	: _transformManager(_entityManager),
	  _rollbackManager(*this, _entityManager)
{
	_playerEntityMap.fill(core::INVALID_ENTITY);
}

void GameManager::SpawnPlayer(const PlayerNumber playerNumber, const core::Vec2f position, const core::Degree rotation)
{
	if (GetEntityFromPlayerNumber(playerNumber) != core::INVALID_ENTITY)
		return;
	core::LogDebug("[GameManager] Spawning new player");
	const auto entity = _entityManager.CreateEntity();
	_playerEntityMap[playerNumber] = entity;

	_transformManager.AddComponent(entity);
	_transformManager.SetPosition(entity, position);
	_transformManager.SetRotation(entity, rotation);
	_rollbackManager.SpawnPlayer(playerNumber, entity, position, rotation);
}

core::Entity GameManager::GetEntityFromPlayerNumber(const PlayerNumber playerNumber) const
{
	return _playerEntityMap[playerNumber];
}


void GameManager::SetPlayerInput(const PlayerNumber playerNumber, const PlayerInput playerInput,
								 const std::uint32_t inputFrame)
{
	if (playerNumber == INVALID_PLAYER)
		return;

	_rollbackManager.SetPlayerInput(playerNumber, playerInput, inputFrame);
}

void GameManager::Validate(const Frame newValidateFrame)
{
	#ifdef TRACY_ENABLE
	ZoneScoped;
	#endif
	if (_rollbackManager.GetCurrentFrame() < newValidateFrame)
	{
		_rollbackManager.StartNewFrame(newValidateFrame);
	}
	_rollbackManager.ValidateFrame(newValidateFrame);
}

core::Entity GameManager::SpawnBullet(const PlayerNumber playerNumber, const core::Vec2f position,
									  const core::Vec2f velocity)
{
	const core::Entity entity = _entityManager.CreateEntity();

	_transformManager.AddComponent(entity);
	_transformManager.SetPosition(entity, position);
	_transformManager.SetScale(entity, core::Vec2f::One() * BULLET_SCALE);
	_transformManager.SetRotation(entity, core::Degree(0.0f));
	_rollbackManager.SpawnBullet(playerNumber, entity, position, velocity);
	return entity;
}

void GameManager::DestroyBullet(const core::Entity entity)
{
	_rollbackManager.DestroyEntity(entity);
}

PlayerNumber GameManager::CheckWinner() const
{
	int alivePlayer = 0;
	PlayerNumber winner = INVALID_PLAYER;
	const auto& playerManager = _rollbackManager.GetPlayerCharacterManager();
	for (core::Entity entity = 0; entity < _entityManager.GetEntitiesSize(); entity++)
	{
		if (!_entityManager.HasComponent(entity, static_cast<core::EntityMask>(ComponentType::PlayerCharacter)))
			continue;
		const auto& player = playerManager.GetComponent(entity);
		if (player.health > 0)
		{
			alivePlayer++;
			winner = player.playerNumber;
		}
	}

	return alivePlayer == 1 ? winner : INVALID_PLAYER;
}

void GameManager::WinGame(const PlayerNumber winner)
{
	_winner = winner;
}

ClientGameManager::ClientGameManager(PacketSenderInterface& packetSenderInterface)
	: GameManager(),
	  _packetSenderInterface(packetSenderInterface),
	  _spriteManager(_entityManager, _transformManager) {}

void ClientGameManager::Begin()
{
	#ifdef TRACY_ENABLE
	ZoneScoped;
	#endif
	//load textures
	if (!_bulletTexture.loadFromFile("data/sprites/bullet.png"))
	{
		core::LogError("Could not load bullet sprite");
	}
	if (!_shipTexture.loadFromFile("data/sprites/ship.png"))
	{
		core::LogError("Could not load ship sprite");
	}
	//load fonts
	if (!_font.loadFromFile("data/fonts/8-bit-hud.ttf"))
	{
		core::LogError("Could not load font");
	}
	_textRenderer.setFont(_font);
	_starBackground.Init();
}

void ClientGameManager::Update(const sf::Time dt)
{
	#ifdef TRACY_ENABLE
	ZoneScoped;
	#endif
	if (_state & Started)
	{
		_rollbackManager.SimulateToCurrentFrame();
		//Copy rollback transform position to our own
		for (core::Entity entity = 0; entity < _entityManager.GetEntitiesSize(); entity++)
		{
			if (_entityManager.HasComponent(entity,
				static_cast<core::EntityMask>(ComponentType::PlayerCharacter) |
				static_cast<core::EntityMask>(core::ComponentType::Sprite)))
			{
				const auto& player = _rollbackManager.GetPlayerCharacterManager().GetComponent(entity);

				if (player.invincibilityTime > 0.0f &&
					std::fmod(player.invincibilityTime, INVINCIBILITY_FLASH_PERIOD) > INVINCIBILITY_FLASH_PERIOD / 2.0f)
				{
					_spriteManager.SetColor(entity, sf::Color::Black);
				}
				else
				{
					_spriteManager.SetColor(entity, PLAYER_COLORS[player.playerNumber]);
				}
			}

			if (_entityManager.HasComponent(entity, static_cast<core::EntityMask>(core::ComponentType::Transform)))
			{
				_transformManager.SetPosition(entity, _rollbackManager.GetTransformManager().GetPosition(entity));
				_transformManager.SetScale(entity, _rollbackManager.GetTransformManager().GetScale(entity));
				_transformManager.SetRotation(entity, _rollbackManager.GetTransformManager().GetRotation(entity));
			}
		}
	}
	_fixedTimer += dt.asSeconds();
	while (_fixedTimer > FIXED_PERIOD)
	{
		FixedUpdate();
		_fixedTimer -= FIXED_PERIOD;
	}
}

void ClientGameManager::End() {}

void ClientGameManager::SetWindowSize(const sf::Vector2u windowsSize)
{
	_windowSize = windowsSize;
	const auto width = static_cast<float>(_windowSize.x);
	const auto height = static_cast<float>(_windowSize.y);
	const sf::FloatRect visibleArea(0.0f, 0.0f, width, height);
	_originalView = sf::View(visibleArea);
	_spriteManager.SetWindowSize(sf::Vector2f(windowsSize));
	_spriteManager.SetCenter(sf::Vector2f(windowsSize) / 2.0f);
	auto& currentPhysicsManager = _rollbackManager.GetCurrentPhysicsManager();
	currentPhysicsManager.SetCenter(sf::Vector2f(windowsSize) / 2.0f);
	currentPhysicsManager.SetWindowSize(sf::Vector2f(windowsSize));
}

void ClientGameManager::Draw(sf::RenderTarget& target)
{
	#ifdef TRACY_ENABLE
	ZoneScoped;
	#endif
	UpdateCameraView();
	target.setView(_cameraView);

	_starBackground.Draw(target);
	_spriteManager.Draw(target);

	if (_drawPhysics)
	{
		auto& currentPhysicsManager = _rollbackManager.GetCurrentPhysicsManager();
		currentPhysicsManager.Draw(target);
	}

	// Draw texts on screen
	target.setView(_originalView);
	if (_state & Finished)
	{
		if (_winner == GetPlayerNumber())
		{
			const std::string winnerText = fmt::format("You won!");
			_textRenderer.setFillColor(sf::Color::White);
			_textRenderer.setString(winnerText);
			_textRenderer.setCharacterSize(32);
			const auto textBounds = _textRenderer.getLocalBounds();
			_textRenderer.setPosition(static_cast<float>(_windowSize.x) / 2.0f - textBounds.width / 2.0f,
				static_cast<float>(_windowSize.y) / 2.0f - textBounds.height / 2.0f);
			target.draw(_textRenderer);
		}
		else if (_winner != INVALID_PLAYER)
		{
			const std::string winnerText = fmt::format("P{} won!", _winner + 1);
			_textRenderer.setFillColor(sf::Color::White);
			_textRenderer.setString(winnerText);
			_textRenderer.setCharacterSize(32);
			const auto textBounds = _textRenderer.getLocalBounds();
			_textRenderer.setPosition(static_cast<float>(_windowSize.x) / 2.0f - textBounds.width / 2.0f,
				static_cast<float>(_windowSize.y) / 2.0f - textBounds.height / 2.0f);
			target.draw(_textRenderer);
		}
		else
		{
			const std::string errorMessage = fmt::format("Error with other players");
			_textRenderer.setFillColor(sf::Color::Red);
			_textRenderer.setString(errorMessage);
			_textRenderer.setCharacterSize(32);
			const auto textBounds = _textRenderer.getLocalBounds();
			_textRenderer.setPosition(static_cast<float>(_windowSize.x) / 2.0f - textBounds.width / 2.0f,
				static_cast<float>(_windowSize.y) / 2.0f - textBounds.height / 2.0f);
			target.draw(_textRenderer);
		}
	}
	if (!(_state & Started))
	{
		if (_startingTime != 0)
		{
			using namespace std::chrono;
			const unsigned long long ms = duration_cast<milliseconds>(
				system_clock::now().time_since_epoch()
			).count();
			if (ms < _startingTime)
			{
				const std::string countDownText = fmt::format("Starts in {}", ((_startingTime - ms) / 1000 + 1));
				_textRenderer.setFillColor(sf::Color::White);
				_textRenderer.setString(countDownText);
				_textRenderer.setCharacterSize(32);
				const auto textBounds = _textRenderer.getLocalBounds();
				_textRenderer.setPosition(static_cast<float>(_windowSize.x) / 2.0f - textBounds.width / 2.0f,
					static_cast<float>(_windowSize.y) / 2.0f - textBounds.height / 2.0f);
				target.draw(_textRenderer);
			}
		}
	}
	else
	{
		std::string health;
		const auto& playerManager = _rollbackManager.GetPlayerCharacterManager();
		for (PlayerNumber playerNumber = 0; playerNumber < MAX_PLAYER_NMB; playerNumber++)
		{
			const auto playerEntity = GetEntityFromPlayerNumber(playerNumber);
			if (playerEntity == core::INVALID_ENTITY)
			{
				continue;
			}
			health += fmt::format("P{} health: {} ", playerNumber + 1, playerManager.GetComponent(playerEntity).health);
		}
		_textRenderer.setFillColor(sf::Color::White);
		_textRenderer.setString(health);
		_textRenderer.setPosition(10, 10);
		_textRenderer.setCharacterSize(20);
		target.draw(_textRenderer);
	}
}

void ClientGameManager::SetClientPlayer(const PlayerNumber clientPlayer)
{
	_clientPlayer = clientPlayer;
}

void ClientGameManager::SpawnPlayer(PlayerNumber playerNumber, const core::Vec2f position, const core::Degree rotation)
{
	core::LogDebug(fmt::format("Spawn player: {}", playerNumber));

	GameManager::SpawnPlayer(playerNumber, position, rotation);
	const auto entity = GetEntityFromPlayerNumber(playerNumber);
	_spriteManager.AddComponent(entity);
	_spriteManager.SetTexture(entity, _shipTexture);
	_spriteManager.SetOrigin(entity, sf::Vector2f(_shipTexture.getSize()) / 2.0f);
	_spriteManager.SetColor(entity, PLAYER_COLORS[playerNumber]);
}

core::Entity ClientGameManager::SpawnBullet(const PlayerNumber playerNumber, const core::Vec2f position,
											const core::Vec2f velocity)
{
	const auto entity = GameManager::SpawnBullet(playerNumber, position, velocity);

	_spriteManager.AddComponent(entity);
	_spriteManager.SetTexture(entity, _bulletTexture);
	_spriteManager.SetOrigin(entity, sf::Vector2f(_bulletTexture.getSize()) / 2.0f);
	_spriteManager.SetColor(entity, PLAYER_COLORS[playerNumber]);

	return entity;
}


void ClientGameManager::FixedUpdate()
{
	#ifdef TRACY_ENABLE
	ZoneScoped;
	#endif
	if (!(_state & Started))
	{
		if (_startingTime != 0)
		{
			using namespace std::chrono;
			const auto ms = duration_cast<duration<unsigned long long, std::milli>>(
				system_clock::now().time_since_epoch()
			).count();
			if (ms > _startingTime)
			{
				_state = _state | Started;
			}
			else
			{
				return;
			}
		}
		else
		{
			return;
		}
	}
	if (_state & Finished)
	{
		return;
	}

	//We send the player inputs when the game started
	const auto playerNumber = GetPlayerNumber();
	if (playerNumber == INVALID_PLAYER)
	{
		//We still did not receive the spawn player packet, but receive the start game packet
		core::LogWarning(fmt::format("Invalid Player Entity in {}:line {}", __FILE__, __LINE__));
		return;
	}

	const auto& inputs = _rollbackManager.GetInputs(playerNumber);
	auto playerInputPacket = std::make_unique<PlayerInputPacket>();
	playerInputPacket->playerNumber = playerNumber;
	playerInputPacket->currentFrame = core::ConvertToBinary(_currentFrame);
	for (size_t i = 0; i < playerInputPacket->inputs.size(); i++)
	{
		if (i > _currentFrame)
		{
			break;
		}

		playerInputPacket->inputs[i] = inputs[i];
	}
	_packetSenderInterface.SendUnreliablePacket(std::move(playerInputPacket));


	_currentFrame++;
	_rollbackManager.StartNewFrame(_currentFrame);
}


void ClientGameManager::SetPlayerInput(const PlayerNumber playerNumber, const PlayerInput playerInput,
									   const std::uint32_t inputFrame)
{
	if (playerNumber == INVALID_PLAYER)
		return;
	GameManager::SetPlayerInput(playerNumber, playerInput, inputFrame);
}

void ClientGameManager::StartGame(unsigned long long int startingTime)
{
	core::LogDebug(fmt::format("Start game at starting time: {}", startingTime));
	_startingTime = startingTime;
}

void ClientGameManager::DrawImGui()
{
	ImGui::Text(_state & Started ? "Game has started" : "Game has not started");
	if (_startingTime != 0)
	{
		ImGui::Text("Starting Time: %llu", _startingTime);
		using namespace std::chrono;
		const unsigned long long ms = duration_cast<milliseconds>(
			system_clock::now().time_since_epoch()
		).count();
		ImGui::Text("Current Time: %llu", ms);
	}
	ImGui::Checkbox("Draw Physics", &_drawPhysics);
}

void ClientGameManager::ConfirmValidateFrame(Frame newValidateFrame,
											 const std::array<PhysicsState, MAX_PLAYER_NMB>& physicsStates)
{
	if (newValidateFrame < _rollbackManager.GetLastValidateFrame())
	{
		core::LogWarning(fmt::format("New validate frame is too old"));
		return;
	}
	for (PlayerNumber playerNumber = 0; playerNumber < MAX_PLAYER_NMB; playerNumber++)
	{
		if (_rollbackManager.GetLastReceivedFrame(playerNumber) < newValidateFrame)
		{
			core::LogWarning(fmt::format(
				"Trying to validate frame {} while playerNumber {} is at input frame {}, client player {}",
				newValidateFrame,
				playerNumber + 1,
				_rollbackManager.GetLastReceivedFrame(playerNumber),
				GetPlayerNumber() + 1));


			return;
		}
	}
	_rollbackManager.ConfirmFrame(newValidateFrame, physicsStates);
}

void ClientGameManager::WinGame(const PlayerNumber winner)
{
	GameManager::WinGame(winner);
	_state = _state | Finished;
}

void ClientGameManager::UpdateCameraView()
{
	if ((_state & Started) != Started)
	{
		_cameraView = _originalView;
		return;
	}

	_cameraView = _originalView;
	const sf::Vector2f extends{_cameraView.getSize() / 2.0f / core::PIXEL_PER_METER};
	float currentZoom = 1.0f;
	for (PlayerNumber playerNumber = 0; playerNumber < MAX_PLAYER_NMB; playerNumber++)
	{
		const auto playerEntity = GetEntityFromPlayerNumber(playerNumber);
		if (playerEntity == core::INVALID_ENTITY)
		{
			continue;
		}
		if (_entityManager.HasComponent(playerEntity, static_cast<core::EntityMask>(core::ComponentType::Position)))
		{
			constexpr float margin = 1.0f;
			const auto position = _transformManager.GetPosition(playerEntity);
			if (core::Abs(position.x) + margin > extends.x)
			{
				const auto ratio = (std::abs(position.x) + margin) / extends.x;
				if (ratio > currentZoom)
				{
					currentZoom = ratio;
				}
			}
			if (core::Abs(position.y) + margin > extends.y)
			{
				const auto ratio = (std::abs(position.y) + margin) / extends.y;
				if (ratio > currentZoom)
				{
					currentZoom = ratio;
				}
			}
		}
	}
	_cameraView.zoom(currentZoom);
}
}