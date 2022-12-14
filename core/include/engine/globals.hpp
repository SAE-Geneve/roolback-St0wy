#pragma once

#include <SFML/System/Vector2.hpp>

namespace core
{
constexpr std::uint32_t WINDOW_SCALE = 100;
const sf::Vector2u WINDOW_RATIO = {16, 9};
const sf::Vector2u WINDOW_SIZE = WINDOW_RATIO * WINDOW_SCALE;
constexpr std::size_t ENTITY_INIT_NMB = 128;
constexpr float PIXEL_PER_METER = 100.0f;
} // namespace core
