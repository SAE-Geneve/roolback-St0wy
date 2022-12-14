#pragma once

#include "manifold.hpp"

#include "engine/component.hpp"
#include "engine/entity.hpp"

#include "maths/vec2.hpp"

#include "transform.hpp"

namespace game
{
struct CircleCollider;
struct AabbCollider;

/**
 * \brief A pure virtual struct that represents colliders.
 */
struct Collider
{
public:
	Collider() = default;
	Collider(Collider&& col) noexcept = default;
	virtual ~Collider() = default;
	Collider& operator=(Collider&& col) = default;
	Collider& operator=(const Collider& col) = default;
	Collider(const Collider& col) = default;

	/**
	* \brief The center of the collider.
	*/
	core::Vec2f center{};

	/**
	 * \brief Tests the collision against a generic collider.
	 * \param transform The transform of this collider.
	 * \param collider The collider to collide with.
	 * \param colliderTransform The transform of the collider to collide with.
	 * \return The manifold of that collision.
	 */
	virtual Manifold TestCollision(
		const Transform* transform,
		const Collider* collider,
		const Transform* colliderTransform
	) const = 0;

	/**
	 * \brief Tests the collision against a circle collider.
	 * \param transform The transform of this collider.
	 * \param collider The circle collider to collide with.
	 * \param circleTransform The transform of the collider to collide with.
	 * \return The manifold of that collision.
	 */
	virtual Manifold TestCollision(
		const Transform* transform,
		const CircleCollider* collider,
		const Transform* circleTransform
	) const = 0;

	/**
	 * \brief Tests the collision against a aabb collider.
	 * \param transform The transform of this collider.
	 * \param collider The circle collider to collide with.
	 * \param aabbTransform The transform of the collider to collide with.
	 * \return The manifold of that collision.
	 */
	virtual Manifold TestCollision(
		const Transform* transform,
		const AabbCollider* collider,
		const Transform* aabbTransform
	) const = 0;

	/**
	 * \brief Find the furthest point in the specified direction.
	 * \param transform The transform of this collider.
	 * \param direction Direction in which to find the furthest point.
	 * \return The furthest point.
	 */
	[[nodiscard]] virtual core::Vec2f FindFurthestPoint(
		const Transform* transform,
		const core::Vec2f& direction
	) const = 0;

	/**
	 * \brief Gets the size of the box that surrounds the collider.
	 * \return The bounding box of the collider.
	 */
	[[nodiscard]] virtual core::Vec2f GetBoundingBoxSize() const = 0;
};

/**
 * \brief A circle collider.
 */
struct CircleCollider final : Collider
{
public:
	/**
	 * \brief Radius of the circle.
	 */
	float radius = 0;

	Manifold TestCollision(
		const Transform* transform,
		const Collider* collider,
		const Transform* colliderTransform
	) const override;

	Manifold TestCollision(
		const Transform* transform,
		const CircleCollider* collider,
		const Transform* circleTransform
	) const override;

	Manifold TestCollision(const Transform* transform, const AabbCollider* collider,
	                       const Transform* aabbTransform) const override;

	[[nodiscard]] core::Vec2f FindFurthestPoint(
		const Transform* transform,
		const core::Vec2f& direction
	) const override;
	[[nodiscard]] core::Vec2f GetBoundingBoxSize() const override;
};

/**
 * \brief An Axis Aligned Bounding Box collider.
 */
struct AabbCollider final : Collider
{
	/**
	 * \brief Half of the width of the box.
	 */
	float halfWidth = 0;
	/**
	 * \brief Half of the height of the box.
	 */
	float halfHeight = 0;

	Manifold TestCollision(const Transform* transform, const Collider* collider,
	                       const Transform* colliderTransform) const override;
	Manifold TestCollision(const Transform* transform, const CircleCollider* collider,
	                       const Transform* circleTransform) const override;
	Manifold TestCollision(const Transform* transform, const AabbCollider* collider,
	                       const Transform* aabbTransform) const override;
	[[nodiscard]] core::Vec2f
	FindFurthestPoint(const Transform* transform, const core::Vec2f& direction) const override;
	[[nodiscard]] core::Vec2f GetBoundingBoxSize() const override;
};

class AabbColliderManager final :
	public core::ComponentManager<AabbCollider, static_cast<core::EntityMask>(core::ComponentType::AabbCollider)>
{
public:
	using ComponentManager::ComponentManager;
};

class CircleColliderManager final :
	public core::ComponentManager<CircleCollider, static_cast<core::EntityMask>(core::ComponentType::CircleCollider)>
{
public:
	using ComponentManager::ComponentManager;
};
}
