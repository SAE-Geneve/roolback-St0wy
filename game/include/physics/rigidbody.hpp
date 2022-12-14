#pragma once

#include "layers.hpp"
#include "engine/component.hpp"
#include "engine/entity.hpp"

#include "physics/manifold_factory.hpp"

namespace game
{
enum class BodyType : std::uint8_t
{
	Static,
	Kinematic,
	Dynamic,
};

/**
* \brief A Rigidbody that has dynamics.
*/
struct Rigidbody
{
	Rigidbody();

	/**
	 * \brief Gets the transform of the body.
	 * \return The transform of the body.
	 */
	[[nodiscard]] Transform& Trans();

	[[nodiscard]] const Transform& Trans() const;

	/**
	 * \brief Sets the transform of the body.
	 * \param transform Transform to set to the body.
	 */
	void SetTransform(const Transform& transform);

	/**
	 * \brief A boolean saying if the collider is a trigger collider.
	 * This means that it will not have a collision response and will only have a callback.
	 * \return If the collider is a trigger collider.
	 */
	[[nodiscard]] bool IsTrigger() const;

	/**
	 * \brief Sets is trigger on the body.
	 * True means that it will not have a collision response and will only have a callback.
	 * \param isTrigger The boolean saying if the collider is trigger.
	 */
	void SetIsTrigger(bool isTrigger);

	/**
	 * \brief Gets the position of the body in the world.
	 * \return The position of the body in the world.
	 */
	[[nodiscard]] const core::Vec2f& Position() const;

	/**
	 * \brief Sets the position of the body in the world.
	 * \param position The new position of the body.
	 */
	void SetPosition(const core::Vec2f& position);

	[[nodiscard]] core::Radian Rotation() const;
	void SetRotation(core::Radian rotation);

	/**
	 * \brief Gets the force of the gravity on this body.
	 * \return The force of the gravity.
	 */
	[[nodiscard]] const core::Vec2f& GravityAcceleration() const;
	/**
	 * \brief Sets the gravity force.
	 * \param gravityAcceleration New gravity force.
	 */
	void SetGravityAcceleration(const core::Vec2f& gravityAcceleration);

	/**
	 * \brief Gets the force on this body.
	 * \return The force on this body.
	 */
	[[nodiscard]] const core::Vec2f& Force() const;
	/**
	 * \brief Adds force to this body.
	 * \param addedForce The force to add to this body.
	 */
	void ApplyForce(const core::Vec2f& addedForce);
	/**
	 * \brief Sets the force of this body.
	 * \param force The new force.
	 */
	void SetForce(const core::Vec2f& force);

	/**
	 * \brief Gets the velocity of this body.
	 * \return The velocity of this body.
	 */
	[[nodiscard]] const core::Vec2f& Velocity() const;
	/**
	 * \brief Sets the velocity of this body.
	 * \param velocity The new velocity.
	 */
	void SetVelocity(const core::Vec2f& velocity);

	/**
	 * \brief Computes the mass of this body. Only the inverted mass is stored,
	 * so it's computed as 1 / InvMass();
	 * \see InvMass()
	 * \return The mass of this body.
	 */
	[[nodiscard]] float Mass() const;
	/**
	 * \brief Returns 1 / Mass of this body.
	 * \return The inverted mass of this body.
	 */
	[[nodiscard]] float InvMass() const;
	/**
	 * \brief Sets the mass of this body.
	 * \param mass The new mass.
	 */
	void SetMass(float mass);

	/**
	 * \brief Gets a boolean indicating whether this body takes gravity.
	 * \return True if this body takes gravity.
	 */
	[[nodiscard]] bool TakesGravity() const;
	/**
	 * \brief Sets a boolean indicating whether this body takes gravity.
	 * \param takesGravity The new takesGravity.
	 */
	void SetTakesGravity(bool takesGravity);

	/**
	 * \brief Gets the static friction of this body.
	 * \return The static friction.
	 */
	[[nodiscard]] float StaticFriction() const;
	/**
	 * \brief Sets the static friction of this body.
	 * \param staticFriction The new static friction.
	 */
	void SetStaticFriction(float staticFriction);

	/**
	 * \brief Gets the dynamic friction of this body.
	 * \return The dynamic friction.
	 */
	[[nodiscard]] float DynamicFriction() const;
	/**
	 * \brief Sets the dynamic friction of this body.
	 * \param dynamicFriction The new dynamic friction.
	 */
	void SetDynamicFriction(float dynamicFriction);

	/**
	 * \brief Gets the restitution of this body. Can be seen as the "Bounciness".
	 * \return The restitution.
	 */
	[[nodiscard]] float Restitution() const;
	/**
	 * \brief Sets the restitution of this body. Can be seen as the "Bounciness".
	 * \param restitution The new restitution.
	 */
	void SetRestitution(float restitution);

	[[nodiscard]] float DragFactor() const { return _dragFactor; }
	void SetDragFactor(const float dragFactor) { _dragFactor = dragFactor; }

	[[nodiscard]] Layer GetLayer() const { return _layer; }
	void SetLayer(const Layer layer) { _layer = layer; }

	[[nodiscard]] bool IsDynamic() const { return _bodyType == BodyType::Dynamic; }
	[[nodiscard]] bool IsStatic() const { return _bodyType == BodyType::Static; }
	[[nodiscard]] bool IsKinematic() const { return _bodyType == BodyType::Kinematic; }
	[[nodiscard]] bool HasCollisions() const { return IsDynamic() || IsKinematic(); }
	[[nodiscard]] BodyType GetBodyType() const { return _bodyType; }
	void SetBodyType(const BodyType bodyType) { _bodyType = bodyType; }

private:
	core::Vec2f _gravityAcceleration;
	core::Vec2f _force;
	core::Vec2f _velocity;

	float _invMass{};
	bool _takesGravity = false;

	float _staticFriction{};
	float _dynamicFriction{};
	float _restitution{};
	float _dragFactor = 1.0f;

	Transform _transform{};

	bool _isTrigger = false;

	BodyType _bodyType = BodyType::Static;
	Layer _layer = Layer::None;
};

/**
 * \brief BodyManager is a ComponentManager that holds all the Body in the world.
 */
class RigidbodyManager final :
	public core::ComponentManager<Rigidbody, static_cast<core::EntityMask>(core::ComponentType::Rigidbody)>
{
public:
	using ComponentManager::ComponentManager;
};
}
