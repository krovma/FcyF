#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/OBBCollider2D.hpp"
#include "Engine/Physics/CapsuleCollider2D.hpp"
#define _USE_MATH_DEFINES
#include <math.h>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
////////////////////////////////
Rigidbody2D::Rigidbody2D(Transform2D* transform)
	: m_entityTransform(transform)
{
	UpdateFromTransform();
}

////////////////////////////////
Rigidbody2D::~Rigidbody2D()
{
	delete m_collider;
}

////////////////////////////////
void Rigidbody2D::Update(float deltaSeconds)
{
	if (m_simulationType == PHSX_SIM_DYNAMIC) {
		m_velocity += m_acceleration * deltaSeconds;
		m_velocity.x *= (1.f - m_restriction.x);
		m_velocity.y *= (1.f - m_restriction.y);
		m_velocity *= (1.f - (GetLinearDrag() * deltaSeconds));

		m_position += m_velocity * deltaSeconds;
		
		m_angularVelocity += m_angularAcceleration * deltaSeconds;
		m_angularVelocity *= (1.f - m_restriction.z);
		m_angularVelocity *= (1.f - (GetAngularDrag()* deltaSeconds));

		m_rotationDegrees += m_angularVelocity * deltaSeconds;
	}
}

////////////////////////////////
void Rigidbody2D::SetSimulationType(PhysicsSimulationType type)
{
	m_simulationType = type;
}

////////////////////////////////
void Rigidbody2D::SetCollider(Collider2D* collider)
{
	m_collider = collider;
	_UpdateInertia();
}

////////////////////////////////
void Rigidbody2D::UpdateToTransform() const
{
	m_entityTransform->Position.x = Lerp(m_position.x, m_entityTransform->Position.x, m_restriction.x);
	m_entityTransform->Position.y = Lerp(m_position.y, m_entityTransform->Position.y, m_restriction.y);
	m_entityTransform->RotationDegrees = Lerp(m_rotationDegrees, m_entityTransform->RotationDegrees, m_restriction.z);
}

////////////////////////////////
void Rigidbody2D::UpdateFromTransform()
{
	m_position = m_entityTransform->Position;
	m_rotationDegrees = m_entityTransform->RotationDegrees;
}

////////////////////////////////
bool Rigidbody2D::IsColliding() const
{
	return m_collider->m_inCollision;
}

////////////////////////////////
void Rigidbody2D::SetColliding(bool isColliding)
{
	m_collider->m_inCollision = isColliding;
}

////////////////////////////////
void Rigidbody2D::SetMassKg(float massKg)
{
	m_massKg = massKg;
	_UpdateInertia();
}

////////////////////////////////
void Rigidbody2D::AddLinearForce(const Vec2& forceN)
{
	m_acceleration += forceN / m_massKg;
}

////////////////////////////////
void Rigidbody2D::AddTorque(float torque)
{
	m_angularAcceleration += ConvertRadiansToDegrees(torque / m_rotaionalInertia);
}

////////////////////////////////
void Rigidbody2D::AddForceAt(const Vec2& forceN, const Vec2& position)
{
	Vec2 disp = position - GetPosition();
	disp.Rotate90Degrees();
	float angularImpulse = forceN.DotProduct(disp);
	AddLinearForce(forceN);
	AddTorque(angularImpulse);
}

void Rigidbody2D::AddImpulseAt(const Vec2& impulse, const Vec2& position)
{
	Vec2 disp = position - GetPosition();
	disp.Rotate90Degrees();
	float angularImpulse = impulse.DotProduct(disp);
	AddImpulse(impulse, angularImpulse);
}

////////////////////////////////
void Rigidbody2D::AddImpulse(const Vec2& linearImpulse, float angularImpulse)
{
	//AddLinearForce(linearImpulse);
	m_velocity += linearImpulse / m_massKg;
	//AddTorque(angularImpulse);
	m_angularVelocity += ConvertRadiansToDegrees(angularImpulse / m_rotaionalInertia);
}

////////////////////////////////
void Rigidbody2D::_UpdateInertia()
{
	if (m_collider->m_type == COLLIDER_OBB2) {
		Vec2 shape = ((OBBCollider2D*)m_collider)->GetLocalShape().GetSize();
		m_rotaionalInertia = (1.f / 12.f) * m_massKg * (shape.x * shape.x + shape.y * shape.y);
	}
	else if (m_collider->m_type == COLLIDER_CAPSULE2) {
		CapsuleCollider2D* collider = (CapsuleCollider2D*)m_collider;
		Capsule2 shape = collider->GetLocalShape();
		float radius = shape.Radius;
		float boxLength = GetDistance(shape.End, shape.Start);
		float boxMass = m_massKg * (radius * boxLength) / (float(M_PI) * radius * radius + radius * boxLength);
		float diskMass = m_massKg - boxMass;
		float iFromDisk = diskMass * radius * radius * 0.5f + diskMass * (0.5f * boxLength) * (0.5f * boxLength);
		float iFromBox = (1.f / 12.f) * boxMass * (radius * radius + boxLength * boxLength);
		m_rotaionalInertia = iFromBox + iFromDisk;
	} else {
		m_rotaionalInertia = m_massKg;
	}
}

float Rigidbody2D::GetLinearDrag() const
{
	return m_linearDrag;
}

void Rigidbody2D::SetLinearDrag(const float linearDrag)
{
	m_linearDrag = linearDrag;
}

float Rigidbody2D::GetAngularDrag() const
{
	return m_angularDrag;
}

void Rigidbody2D::SetAngularDrag(const float angularDrag)
{
	m_angularDrag = angularDrag;
}
