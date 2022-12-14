#include <game/game_manager.hpp>
#include <game/player_character.hpp>

#ifdef TRACY_ENABLE
#include <Tracy.hpp>
#endif

namespace game
{
void PlayerCharacter::CatchBall()
{
	if (hasBall) return;

	hadBall = hasBall;
	hasBall = true;
}

void PlayerCharacter::ThrowBall()
{
	if (!hasBall) return;

	hadBall = hasBall;
	hasBall = false;
}

PlayerCharacterManager::PlayerCharacterManager(core::EntityManager& entityManager, PhysicsManager& physicsManager,
	GameManager& gameManager)
	: ComponentManager(entityManager),
	_physicsManager(physicsManager),
	_gameManager(gameManager)
{}

void PlayerCharacterManager::FixedUpdate(const sf::Time deltaTime)
{
	#ifdef TRACY_ENABLE
	ZoneScoped;
	#endif
	for (PlayerNumber playerNumber = 0; playerNumber < MAX_PLAYER_NMB; playerNumber++)
	{
		const auto playerEntity = _gameManager.GetEntityFromPlayerNumber(playerNumber);
		const bool isPlayer = _entityManager.HasComponent(playerEntity,
			static_cast<core::EntityMask>(ComponentType::PlayerCharacter));
		if (!isPlayer) continue;

		Rigidbody& playerBody = _physicsManager.GetRigidbody(playerEntity);
		// ReSharper disable once CppUseStructuredBinding
		PlayerCharacter& playerCharacter = GetComponent(playerEntity);
		const auto input = playerCharacter.input;

		const bool right = input & player_input_enum::PlayerInput::Right;
		const bool left = input & player_input_enum::PlayerInput::Left;
		const bool up = input & player_input_enum::PlayerInput::Up;
		const bool down = input & player_input_enum::PlayerInput::Down;
		const bool isMoving = (right ^ left) || (up ^ down);

		const auto horizontalForce = ((left ? -1.0f : 0.0f) + (right ? 1.0f : 0.0f)) * PLAYER_SPEED;
		const auto verticalForce = ((down ? -1.0f : 0.0f) + (up ? 1.0f : 0.0f)) * PLAYER_SPEED;
		const core::Vec2f addedForce{ horizontalForce, verticalForce };
		playerBody.ApplyForce(addedForce);

		if (isMoving)
		{
			core::Radian angle = addedForce.Angle(core::Vec2f::Up());
			if (addedForce.x < 0)
				angle = angle * -1.0f;

			playerCharacter.rotation = angle;
			playerBody.SetRotation(playerCharacter.rotation);

			playerCharacter.aimDirection = addedForce.GetNormalized();
		}

		if (input & player_input_enum::PlayerInput::Shoot && playerCharacter.hasBall)
		{
			const auto currentPlayerSpeed = playerBody.Velocity().GetMagnitude();
			const auto ballVelocity = playerCharacter.aimDirection *
				((core::Vec2f::Dot(playerBody.Velocity(), playerCharacter.aimDirection) > 0.0f ? currentPlayerSpeed : 0.0f)
				+ BALL_SPEED);
			const auto ballPosition = playerBody.Position() + playerCharacter.aimDirection * 0.5f + playerBody.
				Position() * deltaTime.asSeconds();
			_gameManager.SpawnBall(ballPosition,
				ballVelocity);
			playerCharacter.ThrowBall();
		}
	}
}

void PlayerCharacterManager::AddComponent(core::Entity entity)
{
	core::LogInfo(fmt::format("Creating player : id{}", entity));
	ComponentManager<PlayerCharacter, 512>::AddComponent(entity);
}
}
