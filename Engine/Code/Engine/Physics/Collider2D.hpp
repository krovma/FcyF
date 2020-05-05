#pragma once
#include <string>
#include <vector>
class RenderContext;
class Rigidbody2D;
struct Collision2D;
struct Rgba;
//////////////////////////////////////////////////////////////////////////
enum Collider2DType
{
	COLLIDER_AABB2,
	COLLIDER_DISK2D,
	COLLIDER_OBB2,
	COLLIDER_CAPSULE2,

	NUM_COLLIDER_2D_TYPE
};

class Collider2D
{
public:
	Collider2D(Collider2DType colliderType, Rigidbody2D* rigidbody);
	virtual void DebugRender(RenderContext* renderer, const Rgba& renderColor) const;
	Collision2D GetCollisionWith(const Collider2D* other) const;
	void UseAsTrigger()
	{
		m_isTrigger = true;
	}

public:
	const Collider2DType m_type;
	Rigidbody2D* m_rigidbody;
	bool m_inCollision = false;
	bool m_destroied = false;
	bool m_isTrigger = false;

public:
	std::string onCollisionEvent = "";
	std::string onEnterEvent = "";
	std::string onLeaveEvent = "";
	std::vector<Collider2D*> m_insideList;
	void AddInside(Collider2D* c);
	void RemoveInside(Collider2D *c);
	void CleanUpInside();
	void MarkDestroy();
};