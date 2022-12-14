#include "physics/rigidbody.hpp"

#include "maths/basic.hpp"

namespace game
{
Rigidbody::Rigidbody()
	: _invMass(1),
	  _takesGravity(true)
{
	//static std::uint64_t staticId = 0;
	//id = staticId++;
}

Transform& Rigidbody::Trans()
{
	return _transform;
}

const Transform& Rigidbody::Trans() const
{
	return _transform;
}

void Rigidbody::SetTransform(const Transform& transform)
{
	_transform = transform;
}

bool Rigidbody::IsTrigger() const
{
	return _isTrigger;
}

void Rigidbody::SetIsTrigger(const bool isTrigger)
{
	_isTrigger = isTrigger;
}

const core::Vec2f& Rigidbody::Position() const
{
	return _transform.position;
}

void Rigidbody::SetPosition(const core::Vec2f& position)
{
	_transform.position = position;
}

core::Radian Rigidbody::Rotation() const
{
	return _transform.rotation;
}

void Rigidbody::SetRotation(const core::Radian rotation)
{
	_transform.rotation = rotation;
}

const core::Vec2f& Rigidbody::GravityAcceleration() const
{
	return _gravityAcceleration;
}

void Rigidbody::SetGravityAcceleration(const core::Vec2f& gravityAcceleration)
{
	if (!TakesGravity()) return;

	_gravityAcceleration = gravityAcceleration;
}

const core::Vec2f& Rigidbody::Force() const
{
	return _force;
}

void Rigidbody::ApplyForce(const core::Vec2f& addedForce)
{
	this->_force += addedForce;
}

void Rigidbody::SetForce(const core::Vec2f& force)
{
	_force = force;
}

const core::Vec2f& Rigidbody::Velocity() const
{
	return _velocity;
}

void Rigidbody::SetVelocity(const core::Vec2f& velocity)
{
	_velocity = velocity;
}

float Rigidbody::Mass() const
{
	return 1.0f / _invMass;
}

float Rigidbody::InvMass() const
{
	return _invMass;
}

void Rigidbody::SetMass(const float mass)
{
	if (core::Equal(mass, 0))
	{
		_invMass = 0;
	}
	_invMass = 1.0f / mass;

	if (std::fpclassify(_invMass) == FP_SUBNORMAL)
	{
		_invMass = std::numeric_limits<float>::min();
	}
}

bool Rigidbody::TakesGravity() const
{
	return _takesGravity;
}

void Rigidbody::SetTakesGravity(const bool takesGravity)
{
	_takesGravity = takesGravity;
	if (!_takesGravity)
	{
		SetGravityAcceleration(core::Vec2f(0, 0));
	}
}

float Rigidbody::StaticFriction() const
{
	return _staticFriction;
}

void Rigidbody::SetStaticFriction(const float staticFriction)
{
	_staticFriction = staticFriction;
}

float Rigidbody::DynamicFriction() const
{
	return _dynamicFriction;
}

void Rigidbody::SetDynamicFriction(const float dynamicFriction)
{
	_dynamicFriction = dynamicFriction;
}

float Rigidbody::Restitution() const
{
	return _restitution;
}

void Rigidbody::SetRestitution(const float restitution)
{
	_restitution = restitution;
}
}
