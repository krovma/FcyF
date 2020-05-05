#pragma once
#include "Engine/Math/Vec2.hpp"
//////////////////////////////////////////////////////////////////////////
class Collider2D;
//////////////////////////////////////////////////////////////////////////

struct Manifold2D
{
	Vec2 normal;
	float penetration;
	Vec2 contactPoint;
};

struct Collision2D
{
	bool isCollide;
	const Collider2D* which;
	const Collider2D* collideWith;
	Manifold2D manifold;
};

bool GetCollision(Collision2D& out_collision, const Collider2D* a, const Collider2D* b);
