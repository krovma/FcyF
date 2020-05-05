#include "Engine/Physics/Collision2D.hpp"
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Physics/AABBCollider2D.hpp"
#include "Engine/Physics/DiskCollider2D.hpp"
#include "Engine/Physics/OBBCollider2D.hpp"
#include "Engine/Physics/CapsuleCollider2D.hpp"

#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <algorithm>

#include "Engine/Develop/DebugRenderer.hpp"
//////////////////////////////////////////////////////////////////////////
using CollideCheck2DFunction = bool(Collision2D&, const Collider2D*, const Collider2D*);
//////////////////////////////////////////////////////////////////////////
/// All manifold generating function assume the Colliders do collide ///
//////////////////////////////////////////////////////////////////////////
bool _SetManifold(Manifold2D& out_manifold, const AABB2& a, const AABB2& b)
{
	AABB2 manifold;
	manifold.Min.x = std::max(a.Min.x, b.Min.x);
	manifold.Min.y = std::max(a.Min.y, b.Min.y);
	manifold.Max.x = std::min(a.Max.x, b.Max.x);
	manifold.Max.y = std::min(a.Max.y, b.Max.y);
	
	if (manifold.Max.AllComponentGreatThan(manifold.Min)) {
		float width = manifold.GetWidth();
		float height = manifold.GetHeight();
		Vec2 displacement = a.GetCenter() - b.GetCenter();
		if (width < height) {
			float dir = (float)Sgn(Vec2(1.f, 0.f).DotProduct(displacement));
			out_manifold.normal = Vec2(dir, 0.f).GetNormalized();
			out_manifold.penetration = width;
		} else {
			float dir = (float)Sgn(Vec2(0.f, 1.f).DotProduct(displacement));
			out_manifold.normal = Vec2(0.f, dir).GetNormalized();
			out_manifold.penetration = height;
		}
		return true;
	} else {
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
bool _SetManifold(Manifold2D& out_manifold, const AABB2& a, const Vec2& center, float radius)
{
	Vec2 nearestPointOnAABB = GetNearestPointOnAABB2(center, a);
	float distance2 = GetDistanceSquare(center, nearestPointOnAABB);
	if (distance2 < radius * radius) {
		out_manifold.normal = (nearestPointOnAABB - center).GetNormalized();
		out_manifold.penetration = radius - sqrtf(distance2);
		return true;
	} else {
		return false;
	}
}
//////////////////////////////////////////////////////////////////////////
bool _SetManifold(Manifold2D& out_manifold, const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB)
{
	Vec2 dispAB = centerA - centerB;
	float distance2 = dispAB.GetLengthSquare();
	if (distance2 < (radiusB + radiusA) * (radiusB + radiusA)) {
		out_manifold.normal = dispAB.GetNormalized();
		out_manifold.penetration = (radiusB + radiusA) - sqrtf(distance2);
		return true;
	} else {
		return false;
	}
}

bool _SetManifold(Manifold2D& out_manifold, const OBB2& boxA, const OBB2& boxB)
{
	if (!boxA.IsIntersectWith(boxB)) {
		return false;
	}

	float dir = boxA.Right.DotProduct(boxB.Right);
	bool pal = (FloatEq(dir , 0.f) || FloatEq(fabsf(dir), 1.f));

	Vec2 cornersA[5];
	{
		boxA.GetCorners(cornersA);
		cornersA[4] = cornersA[0];
	}
	Vec2 cornersB[5];
	{
		boxB.GetCorners(cornersB);
		cornersB[4] = cornersB[0];
	}

	float bestMatch = 1e6f;
	for (int side = 0; side < 4; ++side) {
		Vec2 start = cornersB[side];
		Vec2 end = cornersB[side + 1];

		float minLocal = 1e6f;
		float maxLocal = -1e6f;

		Vec2 localI = (end - start).GetNormalized();
		float maxI = GetDistance(start, end);
		Vec2 minA;
		Vec2 maxA;
		for (int corner = 0; corner < 4; ++corner) {
			Vec2 disp =cornersA[corner] - start;
			float localX = disp.DotProduct(localI);
			if (localX < minLocal) {
				minLocal = localX;
				minA = cornersA[corner];
			}
			if (localX > maxLocal) {
				maxLocal = localX;
				maxA = cornersA[corner];
			}
		}
		float aMinbMax = maxI - minLocal;
		float bMinaMax = maxLocal;
		if ((aMinbMax < 0 && bMinaMax > 0) || (bMinaMax < 0 && bMinaMax > 0)) {
			continue;
		}
		if (aMinbMax < bMinaMax) {

			if (aMinbMax < bestMatch) {
				bestMatch = aMinbMax;
				out_manifold.normal = localI;
				out_manifold.penetration = bestMatch;
				out_manifold.contactPoint = minA;
				if (pal) {
					out_manifold.contactPoint = (minA + end)*0.5f;
				}
			}
		} else {
			if (bMinaMax < bestMatch) {
				bestMatch = bMinaMax;
				out_manifold.normal = -localI;
				out_manifold.penetration = bestMatch;
				out_manifold.contactPoint = maxA;
				if (pal) {
					out_manifold.contactPoint = (maxA + start) * 0.5f;
				}
			}
		}
	}

	for (int side = 0; side < 4; ++side) {
		Vec2 start = cornersA[side];
		Vec2 end = cornersA[side + 1];

		float minLocal = 1e6f;
		float maxLocal = -1e6f;
		Vec2 localI = (end - start).GetNormalized();
		Vec2 minB;
		Vec2 maxB;
		float maxI = GetDistance(start, end);
		for (int corner = 0; corner < 4; ++corner) {
			Vec2 disp =
				cornersB[corner] - start;
			float localX = disp.DotProduct(localI);
			if (localX < minLocal) {
				minLocal = localX;
				minB = cornersB[corner];
			}
			if (localX > maxLocal) {
				maxLocal = localX;
				maxB = cornersB[corner];
			}
		}
		float aMinbMax = maxLocal;
		float bMinaMax = maxI - minLocal;
		//Vec2 bestB;
		if ((aMinbMax < 0 && bMinaMax > 0) || (bMinaMax < 0 && bMinaMax > 0)) {
			continue;
		}
		if (aMinbMax < bMinaMax) {
			if (aMinbMax < bestMatch) {
				bestMatch = aMinbMax;
				out_manifold.normal = localI;
				out_manifold.penetration = bestMatch;
				out_manifold.contactPoint = maxB - localI * bestMatch;
				if (pal) {
					out_manifold.contactPoint = (maxB - localI * bestMatch + start) * 0.5f;
				}
			}
		} else {
			if (bMinaMax < bestMatch) {
				bestMatch = bMinaMax;
				out_manifold.normal = -localI;
				out_manifold.penetration = bestMatch;
				out_manifold.contactPoint = minB + localI * bestMatch;
				if (pal) {
					out_manifold.contactPoint = (minB + localI * bestMatch + end) * 0.5f;
				}
			}
		}
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool _SetManifold(Manifold2D& out_manifold, const OBB2& boxA, float radiusA, const OBB2& boxB, float radiusB)
{
	bool colliding = _SetManifold(out_manifold, boxA, boxB);
	if (colliding) {
		out_manifold.penetration += (radiusA + radiusB);
		out_manifold.contactPoint -= radiusA * out_manifold.normal;
		return true;
	}
	Vec2 cornersA[5];
	{
		boxA.GetCorners(cornersA);
		cornersA[4] = cornersA[0];
	}
	Vec2 cornersB[5];
	{
		boxB.GetCorners(cornersB);
		cornersB[4] = cornersB[0];
	}
	Vec2 bestA;
	Vec2 bestB;
	//Vec2 contact;
	float bestMatch = 1e6f;
	for (int side = 0; side < 4; ++side) {
		for (int corner = 0; corner < 4; ++corner) {
			Vec2 nearestA = GetNearestPointOnSegment2(cornersB[corner], cornersA[side], cornersA[side + 1]);
			Vec2 nearestB = GetNearestPointOnSegment2(cornersA[corner], cornersB[side], cornersB[side + 1]);
			float length2;
			length2 = (nearestA - cornersB[corner]).GetLengthSquare();
			if (length2 < bestMatch) {
				bestA = nearestA;
				bestB = cornersB[corner];
				bestMatch = length2;
			}
			length2 = (nearestB - cornersA[corner]).GetLengthSquare();
			if (length2 < bestMatch) {
				bestA = cornersA[corner];
				bestB = nearestB;
				bestMatch = length2;
			}
		}
	}
	out_manifold.normal = bestA - bestB;
	out_manifold.penetration = radiusA + radiusB - out_manifold.normal.GetLength();
	out_manifold.normal.Normalize();
	out_manifold.contactPoint = bestA - out_manifold.normal * radiusA;
	if (out_manifold.penetration > 0.f) {
		return true;
	}
	out_manifold.penetration = 0.f;
	
	
	return false;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_AABB2_AABB2(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	AABBCollider2D* colliderA = (AABBCollider2D*)a;
	AABBCollider2D* colliderB = (AABBCollider2D*)b;
	AABB2 boxA = colliderA->GetWorldShape();
	AABB2 boxB = colliderB->GetWorldShape();

	out_collision.isCollide = _SetManifold(out_collision.manifold, boxA, boxB);
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_AABB2_Disk(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	AABBCollider2D* colliderA = (AABBCollider2D*)a;
	DiskCollider2D* colliderB = (DiskCollider2D*)b;
	AABB2 boxA = colliderA->GetWorldShape();
	Vec2 centerB = colliderB->m_rigidbody->GetPosition();
	float radiusB = colliderB->GetRadius();
	out_collision.isCollide = _SetManifold(out_collision.manifold, boxA, centerB, radiusB);
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_Disk_AABB2(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	DiskCollider2D* colliderA = (DiskCollider2D*)a;
	AABBCollider2D* colliderB = (AABBCollider2D*)b;
	Vec2 centerA = colliderA->m_rigidbody->GetPosition();
	float radiusA = colliderA->GetRadius();
	AABB2 boxB = colliderB->GetWorldShape();
	out_collision.isCollide = _SetManifold(out_collision.manifold, boxB, centerA, radiusA);
	out_collision.manifold.normal *= -1;
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_Disk_Disk(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	DiskCollider2D* colliderA = (DiskCollider2D*)a;
	DiskCollider2D* colliderB = (DiskCollider2D*)b;
	Vec2 centerA = colliderA->m_rigidbody->GetPosition();
	float radiusA = colliderA->GetRadius();
	Vec2 centerB = colliderB->m_rigidbody->GetPosition();
	float radiusB = colliderB->GetRadius();
	out_collision.isCollide = _SetManifold(out_collision.manifold, centerA, radiusA, centerB, radiusB);
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}

extern RenderContext* g_theRenderer;
//////////////////////////////////////////////////////////////////////////
bool _Collide_OBB2_OBB2(Collision2D& out_collision, const Collider2D*a, const Collider2D* b) {
	OBBCollider2D* colliderA = (OBBCollider2D*)a;
	OBBCollider2D* colliderB = (OBBCollider2D*)b;
	out_collision.isCollide = _SetManifold(out_collision.manifold, colliderA->GetWorldShape(), colliderB->GetWorldShape());
	out_collision.which = a;
	out_collision.collideWith = b;
	if (!out_collision.isCollide) {
		out_collision.manifold.penetration = 0.f;
	}
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_OBB2_Capsule2(Collision2D& out_collision, const Collider2D*a, const Collider2D* b)
{
	OBBCollider2D* colliderA = (OBBCollider2D*)a;
	CapsuleCollider2D* colliderB = (CapsuleCollider2D*)b;
	Capsule2 worldB = colliderB->GetWorldShape();
	out_collision.isCollide = _SetManifold(out_collision.manifold, colliderA->GetWorldShape(), 0, OBB2(worldB), worldB.Radius);
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}
//////////////////////////////////////////////////////////////////////////
bool _Collide_Capsule2_OBB2(Collision2D& out_collision, const Collider2D*a, const Collider2D* b)
{
	CapsuleCollider2D* colliderA = (CapsuleCollider2D*)a;
	OBBCollider2D* colliderB = (OBBCollider2D*)b;
	Capsule2 worldA = colliderA->GetWorldShape();
	out_collision.isCollide = _SetManifold(out_collision.manifold, OBB2(worldA), worldA.Radius, colliderB->GetWorldShape(), 0.f);
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}

//////////////////////////////////////////////////////////////////////////
bool _Collide_Capsule2_Capsule2(Collision2D& out_collision, const Collider2D*a, const Collider2D* b)
{
	CapsuleCollider2D* colliderA = (CapsuleCollider2D*)a;
	CapsuleCollider2D* colliderB = (CapsuleCollider2D*)b;
	Capsule2 worldA = colliderA->GetWorldShape();
	Capsule2 worldB = colliderB->GetWorldShape();
	out_collision.isCollide = _SetManifold(out_collision.manifold, OBB2(worldA), worldA.Radius , OBB2(worldB), worldB.Radius);
	out_collision.which = a;
	out_collision.collideWith = b;
	return out_collision.isCollide;
}

////////////////////////////////
bool GetCollision(Collision2D& out_collision, const Collider2D* a, const Collider2D* b)
{
	static CollideCheck2DFunction* collideFunctions[NUM_COLLIDER_2D_TYPE][NUM_COLLIDER_2D_TYPE] = {
		//				AABB2		, Disk		, OBB2, Capsule2
		/*AABB2*/_Collide_AABB2_AABB2, _Collide_AABB2_Disk, nullptr, nullptr,
		/*Disk*/_Collide_Disk_AABB2,	_Collide_Disk_Disk, nullptr, nullptr,
		/*OBB2*/nullptr, nullptr, _Collide_OBB2_OBB2,_Collide_OBB2_Capsule2,
		/*Capsule*/nullptr,nullptr,_Collide_Capsule2_OBB2, _Collide_Capsule2_Capsule2,
	};
	CollideCheck2DFunction* doCollide = collideFunctions[(int)(a->m_type)][(int)(b->m_type)];
	return doCollide(out_collision, a, b);
}