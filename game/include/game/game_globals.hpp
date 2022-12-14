#pragma once
#include <array>

#include "engine/component.hpp"
#include "engine/entity.hpp"

#include "graphics/color.hpp"

#include "maths/vec2.hpp"


namespace game
{
/**
 * \brief PlayerNumber is a type used to define the number of the player.
 * Starting from 0 to maxPlayerNmb
 */
using PlayerNumber = std::uint8_t;

/**
 * \brief INVALID_PLAYER is an integer constant that defines an invalid player number.
 */
constexpr auto INVALID_PLAYER = std::numeric_limits<PlayerNumber>::max();

/**
 * \brief ClientId is a type used to define the client identification.
 * It is given by the server to clients.
 */
enum class ClientId : std::uint16_t
{
};

constexpr auto INVALID_CLIENT_ID = ClientId{0u};
using Frame = std::uint32_t;

/**
 * \brief maxPlayerNmb is a integer constant that defines the maximum number of player per game
 */
constexpr std::uint32_t MAX_PLAYER_NMB = 2;
constexpr float PLAYER_SPEED = 400.0f;
constexpr float BALL_SPEED = 2.0f;
constexpr float BALL_SCALE = 0.3f;

/**
 * \brief windowBufferSize is the size of input stored by a client. 5 seconds of frame at 50 fps
 */
constexpr std::size_t WINDOW_BUFFER_SIZE = 5ull * 50ull;

/**
 * \brief startDelay is the delay to wait before starting a game in milliseconds
 */
constexpr long long START_DELAY = 3000;

/**
 * \brief maxInputNmb is the number of inputs stored into an PlayerInputPacket
 */
constexpr std::size_t MAX_INPUT_NMB = 50;

/**
 * \brief fixedPeriod is the period used in seconds to start a new FixedUpdate method in the game::GameManager
 */
constexpr float FIXED_PERIOD = 1.0f / 50.0f; //50fps


constexpr std::array PLAYER_COLORS
{
	core::Color::Red(),
	core::Color::Blue(),
	core::Color::Yellow(),
	core::Color::Cyan()
};

constexpr std::array SPAWN_POSITIONS
{
	core::Vec2f(-0.5f, 0.0f),
	core::Vec2f(0.5f, 0.0f),
};

constexpr core::EntityMask OTHER_TYPE = static_cast<core::EntityMask>(core::ComponentType::OtherType);

enum class ComponentType : core::EntityMask
{
	PlayerCharacter = OTHER_TYPE,
	Bullet = OTHER_TYPE << 1u,
	Asteroid = OTHER_TYPE << 2u,
	PlayerInput = OTHER_TYPE << 3u,
	Destroyed = OTHER_TYPE << 4u,
	/**
	 * \brief This is a wall that doesn't move.
	 */
	StaticWall = OTHER_TYPE << 5u,

	/**
	 * \brief This makes the object move down at a certain speed.
	 */
	FallingObject = OTHER_TYPE << 6u,

	/**
	 * \brief This is the door of a falling wall.
	 */
	FallingDoor = OTHER_TYPE << 7u,

	/**
	 * \brief Component for objects that should damage the player on collision.
	 */
	Damager = OTHER_TYPE << 8u,
};

constexpr unsigned WINDOW_SCALE = 60;
const sf::Vector2u DEBUG_WINDOW_SIZE{core::WINDOW_RATIO.x * 2 * WINDOW_SCALE, core::WINDOW_RATIO.y * WINDOW_SCALE};
const sf::Vector2u DEBUG_FRAMEBUFFER_SIZE = core::WINDOW_RATIO * WINDOW_SCALE;

/**
 * \brief Color of the background wall of a falling wall.
 */
constexpr core::Color WALL_COLOR{255, 0, 243};
/**
 * \brief Color of the door if the player needs a ball to open it.
 */
constexpr core::Color BALL_DOOR_COLOR{6, 255, 0};
/**
 * \brief Color of the door if the player should not have the ball to open it.
 */
constexpr core::Color NO_BALL_DOOR_COLOR{0, 234, 251};

constexpr core::Vec2f VERTICAL_WALLS_SIZE{1, 100};
constexpr core::Vec2f HORIZONTAL_WALLS_SIZE{100, 0.4f};
constexpr core::Vec2f MIDDLE_WALL_SIZE{0.1f, 100};
constexpr core::Vec2f WALL_LEFT_POS{-9.0f, 0.0f};
constexpr core::Vec2f WALL_RIGHT_POS{9.0f, 0.0f};
constexpr core::Vec2f WALL_BOTTOM_POS{0.0f, -4.1f};
constexpr core::Vec2f WALL_TOP_POS{0.0f, 4.9f};
constexpr core::Vec2f WALL_MIDDLE_POS{0.0f, 0.0f};

constexpr core::Vec2f FALLING_WALL_SIZE{100.0f, 0.3f};
/**
 * \brief Size of by how much the door should be bigger than the background wall.
 */
constexpr float FALLING_WALL_DOOR_COLLIDER_OFFSET = 0.2f;
constexpr core::Vec2f FALLING_WALL_DOOR_SIZE{1.5f, FALLING_WALL_SIZE.y + FALLING_WALL_DOOR_COLLIDER_OFFSET};

/**
 * \brief By how much should the score increment when a falling wall is destroyed.
 */
constexpr std::uint32_t DESTROY_WALL_SCORE_INCREMENT = 100u;

/**
 * \brief PlayerInput is a type defining the input data from a player.
 */
using PlayerInput = std::uint8_t;

namespace player_input_enum
{
enum PlayerInput : std::uint8_t
{
	None = 0u,
	Up = 1u << 0u,
	Down = 1u << 1u,
	Left = 1u << 2u,
	Right = 1u << 3u,
	Shoot = 1u << 4u,
};
}
}
