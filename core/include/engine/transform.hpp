#pragma once
#include <engine/entity.hpp>

#include "component.hpp"

#include "maths/angle.hpp"
#include "maths/vec2.hpp"

namespace core
{
/**
 * \brief PositionManager is a ComponentManager that holds positions in 2d space.
 * Positions are in physics space (using pixelPerMeter in globals.h)
 */
class PositionManager final : public ComponentManager<Vec2f, static_cast<Component>(ComponentType::Position)>
{
public:
	using ComponentManager::ComponentManager;
};

/**
 * \brief ScaleManager is a ComponentManager that holds the scale ratio in x and y.
 * By default, AddComponent will set the scale value to one.
 */
class ScaleManager final : public ComponentManager<Vec2f, static_cast<Component>(ComponentType::Scale)>
{
public:
	using ComponentManager::ComponentManager;
	void AddComponent(Entity entity) override;
};

/**
 * \brief RotationManager is a ComponentManager that holds the Degree angle.
 */
class RotationManager final : public ComponentManager<Degree, static_cast<Component>(ComponentType::Rotation)>
{
public:
	using ComponentManager::ComponentManager;
};

/**
 * \brief TransformManager is a class combining a PositionManager, a ScaleManager and a RotationManager in one.
 */
class TransformManager
{
public:
	explicit TransformManager(EntityManager& entityManager);

	[[nodiscard]] Vec2f GetPosition(Entity entity) const;
	[[nodiscard]] const std::vector<Vec2f>& GetAllPositions() const;
	void SetPosition(Entity entity, Vec2f position);

	[[nodiscard]] Vec2f GetScale(Entity entity) const;
	[[nodiscard]] const std::vector<Vec2f>& GetAllScales() const;
	void SetScale(Entity entity, Vec2f scale);

	[[nodiscard]] Degree GetRotation(Entity entity) const;
	[[nodiscard]] const std::vector<Degree>& GetAllRotations() const;
	void SetRotation(Entity entity, Degree rotation);

	void AddComponent(Entity entity);
	void RemoveComponent(Entity entity);

private:
	PositionManager _positionManager;
	ScaleManager _scaleManager;
	RotationManager _rotationManager;
};
}
