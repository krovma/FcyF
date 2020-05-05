#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Transform2D.hpp"
#include "Engine/Math/Vec3.hpp"
class Collider2D;

enum PhysicsSimulationType
{
	PHSX_SIM_STATIC,
	PHSX_SIM_DYNAMIC,

	NUM_PHSX_SIM_TYPES
};
/*
class Rigidbody2D : public Entity
{
public:
	friend class PhysicsSystem;
	Rigidbody2D(Game* theGame);
	virtual void Update(float deltaSeconds);
	virtual void Render() const;
	const Collider2D* GetCollider() const { return m_collider; }

	void SetSimulationType(PhysicsSimulationType type);
	PhysicsSimulationType GetSimulationType() const { return m_simulationType; }
private:
	Collider2D* m_collider = nullptr;
	PhysicsSimulationType m_simulationType = PHSX_SIM_STATIC;
};
*/
class Rigidbody2D
{
public:
	friend class PhysicsSystem;
	Rigidbody2D(Transform2D* transform);
	~Rigidbody2D();
	void Update(float deltaSeconds);

	Collider2D* GetCollider() const { return m_collider; }
	PhysicsSimulationType GetSimulationType() const { return m_simulationType; }
	void SetSimulationType(PhysicsSimulationType type);
	void SetCollider(Collider2D* collider);
	void UpdateToTransform() const;
	void UpdateFromTransform();
	float GetRotationDegrees() const { return m_rotationDegrees; }
	Vec2 GetPosition() const { return m_position; }
	Vec2 GetVelocity() const { return m_velocity; }
	Vec2 GetAcceleration() const { return m_acceleration; }
	Vec3 GetRestriction() const { return m_restriction; }
	void SetRestriction(const Vec3& restriction) { m_restriction = restriction; }
	float GetAngularSpeed() const { return m_angularVelocity; }
	float GetAngularAcceleration() const { return m_angularAcceleration; }
	bool IsColliding() const;
	void SetColliding(bool isColliding);

	float GetMassKg() const { return m_massKg; }
	float GetRotationalInertia() const { return m_rotaionalInertia; }
	float GetBounciness() const { return m_bounciness; }
	float GetFriction() const { return m_friction; }

	void SetMassKg(float massKg);
	void SetBounciness(float bounce) { m_bounciness = bounce; }
	void SetFriction(float friction) { m_friction = friction; }

	void AddLinearForce(const Vec2& forceN);
	void AddTorque(float torque);
	void AddForceAt(const Vec2& forceN, const Vec2& position);
	void AddImpulseAt(const Vec2& impulse, const Vec2& position);
	void AddImpulse(const Vec2& linearImpulse, float angularImpulse);

	void SetXRestriction(float lock) { m_restriction.x = lock; }
	void SetYRestriction(float lock) { m_restriction.y = lock; }
	void SetZRestriction(float lock) { m_restriction.z = lock; }

private:
	// These setters can only be called from PhysicsSystem
	void SetRotation(float rotationDegrees) { m_rotationDegrees = rotationDegrees; }
	void SetPosition(const Vec2& position) { m_position = position; }
	void SetVelocity(const Vec2& velocity) { m_velocity = velocity; }
	void Move(const Vec2& displacement) { m_position += displacement; }
	void SetAcceleration(const Vec2& acceleration) { m_acceleration = acceleration; }
	
	void _UpdateInertia();

private:
	Collider2D* m_collider = nullptr;
	Transform2D* m_entityTransform;

	Vec2 m_position = Vec2::ZERO;
	float m_rotationDegrees = 0.f;
	Vec2 m_velocity = Vec2::ZERO;
	Vec2 m_acceleration = Vec2::ZERO;
	float m_angularVelocity = 0.f;
	float m_angularAcceleration = 0.f;
	PhysicsSimulationType m_simulationType = PHSX_SIM_STATIC;

	float m_massKg = 1.f;
	float m_bounciness = 1.f;
	float m_friction = 0.f;

	float m_rotaionalInertia = 1.f;

	Vec3 m_restriction = Vec3::ZERO;
	float m_linearDrag = 0.f;
public:
	float GetLinearDrag() const;
	void SetLinearDrag(const float linearDrag);
	float GetAngularDrag() const;
	void SetAngularDrag(const float angularDrag);
private:
	float m_angularDrag = 0.f;

public:
	bool m_isGarbage = false;
};