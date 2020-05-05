#include "Engine/Physics/PhysicsSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Physics/AABBCollider2D.hpp"
#include "Engine/Physics/DiskCollider2D.hpp"
#include "Engine/Physics/OBBCollider2D.hpp"
#include "Engine/Physics/CapsuleCollider2D.hpp"
#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Event/EventSystem.hpp"
//////////////////////////////////////////////////////////////////////////
STATIC Vec2 PhysicsSystem::GRAVATY(0, -9.8f);

const float PhysicsSystem::PHYSICS_TIME_UNIT = 1.f / 60.f;

/////////////////////////
static float __combineFriction(float fa, float fb);

////////////////////////////////
PhysicsSystem::PhysicsSystem()
{

}

////////////////////////////////
PhysicsSystem::~PhysicsSystem()
{
	Shutdown();
}

////////////////////////////////
void PhysicsSystem::Startup()
{

}

////////////////////////////////
void PhysicsSystem::Shutdown()
{
	for (auto eachRigidbody : m_rigidbodies) {
		delete eachRigidbody;
	}
	m_rigidbodies.clear();
}

////////////////////////////////
void PhysicsSystem::BeginFrame()
{
	for (auto eachRigidbody : m_rigidbodies) {
		eachRigidbody->m_collider->m_inCollision = false;
	}
}

////////////////////////////////
void PhysicsSystem::Update(float deltaSeconds)
{
	// Pre update
	for (auto eachRigidbody : m_rigidbodies) {
		eachRigidbody->UpdateFromTransform();
	}
	for (auto eachRigidbody : m_triggers) {
		eachRigidbody->UpdateFromTransform();
	}
	m_accumulatedTime += deltaSeconds;
	if (m_accumulatedTime < PHYSICS_TIME_UNIT) {
		return;
	}
	// Begin update

	// Move every one
	for (auto eachRigidbody : m_rigidbodies) {
		if (eachRigidbody->GetSimulationType() == PHSX_SIM_DYNAMIC) {
			eachRigidbody->Update(m_accumulatedTime);
			eachRigidbody->m_acceleration = Vec2::ZERO;
			eachRigidbody->m_angularAcceleration = 0.f;
			eachRigidbody->AddLinearForce(GRAVATY * eachRigidbody->m_massKg);
		} else {
			eachRigidbody->m_acceleration = Vec2::ZERO;
			eachRigidbody->m_velocity = Vec2::ZERO;
		}
	}

	for (auto eachRigidbody : m_triggers) {
		if (eachRigidbody->GetSimulationType() == PHSX_SIM_DYNAMIC) {
			eachRigidbody->Update(m_accumulatedTime);
			eachRigidbody->m_acceleration = Vec2::ZERO;
			eachRigidbody->m_angularAcceleration = 0.f;
			eachRigidbody->AddLinearForce(GRAVATY * eachRigidbody->m_massKg);
		} else {
			eachRigidbody->m_acceleration = Vec2::ZERO;
			eachRigidbody->m_velocity = Vec2::ZERO;
		}
	}

	//
	_DoDynamicVsStatic(true);
	//_DoDynamicVsDynamic(false);
	_DoDynamicVsDynamic(true);
	_DoDynamicVsStatic(false);
	_DoStaticVsStatic();

	_UpdateTriggers();

	// After update

	for (auto eachRigidbody : m_rigidbodies) {
		eachRigidbody->UpdateToTransform();
		eachRigidbody->UpdateFromTransform();
	}
	for (auto eachRigidbody : m_triggers) {
		eachRigidbody->UpdateToTransform();
		eachRigidbody->UpdateFromTransform();
	}
	m_accumulatedTime = 0.f;
}

////////////////////////////////
void PhysicsSystem::DebugRender(RenderContext* renderer) const
{
	UNUSED(renderer);
}

////////////////////////////////
void PhysicsSystem::EndFrame()
{

}

////////////////////////////////
Rigidbody2D* PhysicsSystem::NewRigidbody2D(
	Collider2DType colliderType
	, NamedStrings& colliderInfo
	, Transform2D* entityTransform
	, PhysicsSimulationType simulation/*=PHSX_SIM_STATIC*/)
{
	Rigidbody2D* createdRigidbody2D = new Rigidbody2D(entityTransform);

	if (colliderType == Collider2DType::COLLIDER_AABB2) {
		AABB2 colliderLocalShape = colliderInfo.GetAABB2("localShape", AABB2::UNIT);
		createdRigidbody2D->m_collider = new AABBCollider2D(colliderLocalShape, createdRigidbody2D);
	} else if (colliderType == Collider2DType::COLLIDER_DISK2D) {
		float colliderRadius = colliderInfo.GetFloat("radius", 0.f);
		createdRigidbody2D->m_collider = new DiskCollider2D(colliderRadius, createdRigidbody2D);
	} else if (colliderType == Collider2DType::COLLIDER_OBB2) {
		float rotation = colliderInfo.GetFloat("rotation", 0.f);
		Vec2 size = colliderInfo.GetVec2("size", Vec2::ZERO);
		createdRigidbody2D->m_collider = new OBBCollider2D(
			OBB2(Vec2::ZERO, size, rotation), createdRigidbody2D
		);
	} else if (colliderType == Collider2DType::COLLIDER_CAPSULE2) {
		Vec2 start = colliderInfo.GetVec2("start", Vec2::ZERO);
		Vec2 end = colliderInfo.GetVec2("end", Vec2::ZERO);
		float radius = colliderInfo.GetFloat("radius", 0.f);

		Capsule2 localShapde = Capsule2(start, end, radius);
		localShapde.SetPosition(Vec2::ZERO);

		createdRigidbody2D->m_collider = new CapsuleCollider2D(localShapde, createdRigidbody2D);
	}
	createdRigidbody2D->SetSimulationType(simulation);
	m_rigidbodies.push_back(createdRigidbody2D);
	return createdRigidbody2D;
}

////////////////////////////////
void PhysicsSystem::DeleteRigidbody2D(Rigidbody2D* rigidbody)
{
	for (auto each = m_rigidbodies.begin(); each != m_rigidbodies.end();) {
		if (*each == rigidbody) {
			rigidbody->m_isGarbage = true;
			rigidbody->m_collider->MarkDestroy();
			return;
		}
		++each;
	}
}

void PhysicsSystem::UseAsTrigger(Rigidbody2D* trigger)
{
	trigger->m_collider->UseAsTrigger();
	for (auto it = m_rigidbodies.begin(); it != m_rigidbodies.end(); ++it) {
		if (*it == trigger) {
			it = m_rigidbodies.erase(it);
			break;
		}
	}
	for (auto it = m_triggers.begin(); it != m_triggers.end(); ++it) {
		if (*it == trigger) {
			return;
		}
	}
	m_triggers.push_back(trigger);
}

void PhysicsSystem::cleanup()
{
	for (auto it = m_rigidbodies.begin(); it != m_rigidbodies.end();) {
		if ((*it)->m_collider->m_destroied) {
			for (auto each : m_triggers) {
				each->m_collider->RemoveInside((*it)->m_collider);
			}
			delete *it;
			it = m_rigidbodies.erase(it);
		} else {
			++it;
		}
	}

	for (auto it = m_triggers.begin(); it != m_triggers.end();) {
		if ((*it)->m_collider->m_destroied) {
			for (auto each : m_triggers) {
				if (each == *it) {
					continue;
				}
				each->m_collider->RemoveInside((*it)->m_collider);
			}
			delete *it;
			it = m_triggers.erase(it);
		} else {
			++it;
		}
	}
}

#include "Engine/Develop/DebugRenderer.hpp"

////////////////////////////////
void PhysicsSystem::_DoDynamicVsStatic(bool isResolve)
{
	for (auto eachDynamic : m_rigidbodies) {
		if (eachDynamic->GetSimulationType() == PHSX_SIM_DYNAMIC) {
			for (auto eachStatic : m_rigidbodies) {
				if (eachStatic->GetSimulationType() == PHSX_SIM_STATIC) {
					const Collider2D* colliderA = eachDynamic->GetCollider();
					const Collider2D* colliderB = eachStatic->GetCollider();
					Collision2D result = colliderA->GetCollisionWith(colliderB);
					//DebugRenderer::DrawPoint3D(Vec3(result.manifold.contactPoint, .1f), 2.f, 1.f, ColorGradient::FADEOUT);
					if (!colliderA->m_isTrigger && !colliderB->m_isTrigger) {
						eachDynamic->Move(result.manifold.normal * result.manifold.penetration);
						if (result.isCollide) {
							eachDynamic->SetColliding(true);
							eachStatic->SetColliding(true);
							if (isResolve) {
								Vec2 contactPointA = result.manifold.contactPoint;
								Vec4 j = _GetCollisionImpulse(result, contactPointA);
								//DebugRenderer::DrawArrow3D(Vec3(contactPointA, 0.1f), Vec3(contactPointA, 0.1f) + Vec3(j * 0.1f, 0.1f), 0.2f, 0.5f, 1.f);
								eachDynamic->AddImpulseAt(Vec2(j.x, j.y), contactPointA);
								eachDynamic->AddImpulseAt(Vec2(j.z, j.w), contactPointA);
								if(!colliderA->onCollisionEvent.empty()) {
									NamedStrings param;
									param.Set("collision", Stringf("%I64d", &result));
									g_Event->Trigger(colliderA->onCollisionEvent, param);
								}
								if (!colliderB->onCollisionEvent.empty()) {
									NamedStrings param;
									Collision2D resultB = result;
									resultB.collideWith = colliderA;
									resultB.manifold.normal *= -1;
									resultB.which = colliderB;
									param.Set("collision", Stringf("%I64d", &resultB));
									g_Event->Trigger(colliderB->onCollisionEvent, param);
								}
							}
						}
					}
				}
			}
		}
	}
}

////////////////////////////////
void PhysicsSystem::_DoDynamicVsDynamic(bool isResolve)
{
	size_t size = m_rigidbodies.size();

	for (size_t i = 0; i < size; ++i) {
		Rigidbody2D* dynamicA = m_rigidbodies[i];
		if (dynamicA->GetSimulationType() == PHSX_SIM_DYNAMIC) {
			for (size_t j = i + 1; j < size; ++j) {
				Rigidbody2D* dynamicB = m_rigidbodies[j];
				if (dynamicB->GetSimulationType() == PHSX_SIM_DYNAMIC) {
					const Collider2D* colliderA = dynamicA->GetCollider();
					const Collider2D* colliderB = dynamicB->GetCollider();
					Collision2D result = colliderA->GetCollisionWith(colliderB);
					Collision2D resultB = colliderB->GetCollisionWith(colliderA);
					Vec2 fullMove = result.manifold.normal * result.manifold.penetration;
					Vec2 movingA = fullMove * (
						dynamicA->m_massKg / (dynamicA->m_massKg + dynamicB->m_massKg)
						);
					Vec2 movingB = -fullMove * (
							dynamicB->m_massKg / (dynamicA->m_massKg + dynamicB->m_massKg)
						);
					dynamicA->Move(movingA);
					dynamicB->Move(movingB);
					if (result.isCollide) {
						dynamicA->SetColliding(true);
						dynamicB->SetColliding(true);
						if (isResolve) {
							
							Vec2 contactPointA = result.manifold.contactPoint + result.manifold.normal * dynamicA->m_massKg / (dynamicA->m_massKg + dynamicB->m_massKg);
							Vec2 contactPointB = resultB.manifold.contactPoint + resultB.manifold.normal * dynamicB->m_massKg / (dynamicA->m_massKg + dynamicB->m_massKg);
							Vec4 jA = _GetCollisionImpulse(result, contactPointA);
							//DebugRenderer::DrawArrow3D(Vec3(contactPointA, 0.1f), Vec3(contactPointA, 0.1f) + Vec3(jA * 0.1f, 0.1f), 0.2f, 0.5f, 1.f);
							dynamicA->AddImpulseAt(Vec2(jA.x, jA.y), contactPointA);
							dynamicA->AddImpulseAt(Vec2(jA.z, jA.w), contactPointA);

							//Vec2 jB = _GetCollisionImpulse(resultB, contactPointB);
							//DebugRenderer::DrawArrow3D(Vec3(contactPointB, 0.1f), Vec3(contactPointB, 0.1f) - Vec3(jA * 0.1f, 0.1f), 0.2f, 0.5f, 0.f);

							dynamicB->AddImpulseAt(-Vec2(jA.x, jA.y), contactPointB);
							dynamicB->AddImpulseAt(-Vec2(jA.z, jA.w), contactPointB);

							if (!colliderA->onCollisionEvent.empty()) {
								NamedStrings param;
								param.Set("collision", Stringf("%I64d", &result));
								g_Event->Trigger(colliderA->onCollisionEvent, param);
							}

							if (!colliderB->onCollisionEvent.empty()) {
								NamedStrings param;
								param.Set("collision", Stringf("%I64d", &resultB));
								g_Event->Trigger(colliderB->onCollisionEvent, param);
							}
						}
					}
				}
			}
		}
	}
}

////////////////////////////////
void PhysicsSystem::_DoStaticVsStatic()
{
	for (auto staticA : m_rigidbodies) {
		if (staticA->GetSimulationType() == PHSX_SIM_STATIC) {
			for (auto staticB : m_rigidbodies) {
				if (staticA == staticB) {
					continue;
				}
				if (staticB->GetSimulationType() == PHSX_SIM_STATIC) {
					const Collider2D* colliderA = staticA->GetCollider();
					const Collider2D* colliderB = staticB->GetCollider();
					Collision2D result = colliderA->GetCollisionWith(colliderB);
					//DebugRenderer::DrawPoint3D(Vec3(result.manifold.contactPoint, .1f), 2.f, 1.f / 30.f, Rgba::LIME);
					if (result.isCollide) {
						staticA->SetColliding(true);
						staticB->SetColliding(true);
						break;
					}
				}
			}
		}
	}
}

void PhysicsSystem::_UpdateTriggers()
{
	for (auto eachTrigger : m_triggers) {
		const auto colliderTg = eachTrigger->m_collider;
		std::vector<Collider2D*> removeList;
		for (auto eachInside : colliderTg->m_insideList) {
			auto result = colliderTg->GetCollisionWith(eachInside);
			if (!result.isCollide) {
				removeList.push_back(eachInside);
			}
		}
		for(auto eachRemove:removeList) {
			colliderTg->RemoveInside(eachRemove);
		}
		for (auto eachRigid : m_rigidbodies) {
			if (fabsf(eachRigid->m_entityTransform->Position.x) > 1e6) {
				continue;
			}
			const auto colliderRb = eachRigid->m_collider;
			Collision2D result = colliderTg->GetCollisionWith(colliderRb);
			if (result.isCollide) {
				colliderTg->AddInside(colliderRb);
			}
		}
		for (auto eachTg : m_triggers) {
			if (eachTg == eachTrigger) {
				continue;
			}
			const auto colliderRb = eachTg->m_collider;
			Collision2D result = colliderTg->GetCollisionWith(colliderRb);
			if (result.isCollide) {
				colliderTg->AddInside(colliderRb);
			}
		}
	}
}

////////////////////////////////
Vec2 PhysicsSystem::_GetElasticCollidedVelocity(const Collision2D& collision) const
{
	const Rigidbody2D* which = collision.which->m_rigidbody;
	const Rigidbody2D* with = collision.collideWith->m_rigidbody;
	float restitution = which->m_bounciness * with->m_bounciness;
	float smoothness = which->m_friction * with->m_friction;
	const Vec2& normal = collision.manifold.normal;
	if (with->m_simulationType == PHSX_SIM_STATIC) {
		Vec2 velocity = which->m_velocity;
		Vec2 normalVelocity = normal * velocity.DotProduct(normal);
		Vec2 tangentVelocity = velocity - normalVelocity;
		return (-normalVelocity * restitution + tangentVelocity * smoothness);
	} else {
		Vec2 vA = which->m_velocity;
		Vec2 vB = with->m_velocity;
		Vec2 vNormalA = normal * vA.DotProduct(normal);
		Vec2 vNormalB = normal * vB.DotProduct(normal);
		Vec2 vTangentA = vA - vNormalA;
		//Vec2 vTangentB = vB - vNormalB;
		float u1 = vNormalA.DotProduct(normal);//vNormalA.GetLength() * Sgn(vA.DotProduct(normal));
		float u2 = vNormalB.DotProduct(normal);//vNormalB.GetLength() * Sgn(vB.DotProduct(normal));
		float m1 = which->m_massKg;
		float m2 = with->m_massKg;
		float v1 = (restitution * m2 * (u2 - u1) + m1 * u1 + m2 * u2) / (m1 + m2);
		//float v2 = (2.f * m1 * u1 + (m2 - m1) * u2) / (m1 + m2);
		vNormalA = v1 * normal;
		
		return (vNormalA + vTangentA * smoothness);
	}
}

////////////////////////////////
Vec4 PhysicsSystem::_GetCollisionImpulse(const Collision2D& collision, const Vec2& contactPoint) const
{
	const Rigidbody2D* which = collision.which->m_rigidbody;
	const Rigidbody2D* with = collision.collideWith->m_rigidbody;
	const Vec2& normal = collision.manifold.normal;
	const Vec2& tangent = normal.GetRotated90Degrees();
	const float restitution = which->m_bounciness * with->m_bounciness;

	const float friction = __combineFriction(which->m_friction, with->m_friction);

/*	float smoothness = which->m_friction * with->m_friction;*/
	float mA = which->m_massKg;
	float mB = with->m_massKg;
 	Vec2 rA = contactPoint - which->m_position;
 	Vec2 rB = contactPoint - with->m_position;

	Vec2 vA = which->m_velocity 
		+ ConvertDegreesToRadians(which->m_angularVelocity) * rA.GetLength()
			* rA.GetNormalized().GetRotated90Degrees();
	Vec2 vB = with->m_velocity
		+ ConvertDegreesToRadians(with->m_angularVelocity) * rB.GetLength()
		* rB.GetNormalized().GetRotated90Degrees();

 	Vec2 vR = vB - vA;
	float cA = rA.GetRotated90Degrees().DotProduct(normal);
	float cB = rB.GetRotated90Degrees().DotProduct(normal);
	//DebugRenderer::DrawArrow3D(Vec3(contactPoint, 0.1f), Vec3(contactPoint, 0.1f) + Vec3(normal * 10.f), 0.2f, 0.5f, 1.f, Rgba::RED);

	Vec4 result;
	float normalImpMag;
	float tangentalImpMag;
	Vec2 normalImp;
	Vec2 tangentalImp;

	if (with->m_simulationType == PHSX_SIM_STATIC) {
		normalImpMag = -(1.f + restitution) * vA.DotProduct(normal) / (
			1.f / mA + cA * cA / which->m_rotaionalInertia
			);
	}
	else {
		normalImpMag = (1.f + restitution) * (vR.DotProduct(normal)) /
			((1.f / mA)
				+ (1.f / mB)
				+ (cA * cA / which->m_rotaionalInertia)
				+ (cB * cB / with->m_rotaionalInertia)
				);
	}

	float tA = rA.GetRotated90Degrees().DotProduct(tangent);
	float tB = rB.GetRotated90Degrees().DotProduct(tangent);

	if (with->m_simulationType == PHSX_SIM_STATIC) {
		tangentalImpMag = -(1.f + restitution) * vA.DotProduct(tangent) / (
			1.f / mA + tA * tA / which->m_rotaionalInertia
			);
	} else {
		tangentalImpMag = (1.f + restitution) * (vR.DotProduct(tangent)) /
			((1.f / mA)
				+ (1.f / mB)
				+ (tA * tA / which->m_rotaionalInertia)
				+ (tB * tB / with->m_rotaionalInertia)
				);
	}
	if ( fabsf(tangentalImpMag * friction) > fabsf(friction * normalImpMag)) {
		tangentalImpMag = Sgn(tangentalImpMag) * fabsf(normalImpMag) * friction;
	} else {
		tangentalImpMag *= friction;
	}
	normalImp = normalImpMag * normal;
	tangentalImp = tangentalImpMag * tangent;
	return Vec4(normalImp.x, normalImp.y, tangentalImp.x, tangentalImp.y);
	//return (mA * mB) / (mA + mB) * (1.f + restitution) * (vB.DotProduct(normal) * normal - vA.DotProduct(normal) * normal);
}

float __combineFriction(float fa, float fb)
{
	return sqrtf(fa * fb);
}
