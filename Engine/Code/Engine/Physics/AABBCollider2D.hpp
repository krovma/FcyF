#pragma once
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/AABB2.hpp"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class AABBCollider2D : public Collider2D
{
public:
	AABBCollider2D(const AABB2& localShape, Rigidbody2D* rigidbody);
	AABB2 GetLocalShape() const;
	AABB2 GetWorldShape() const;

	virtual void DebugRender(RenderContext* renderer, const Rgba& renderColor) const override;
private:
	AABB2 m_localShape;
};