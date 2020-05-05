#pragma once
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/Capsule2.hpp"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class CapsuleCollider2D : public Collider2D
{
public:
	CapsuleCollider2D(const Capsule2& localShape, Rigidbody2D* rigidbody);
	Capsule2 GetLocalShape() const;
	Capsule2 GetWorldShape() const;

	virtual void DebugRender(RenderContext* renderer, const Rgba& renderColor) const override;
private:
	Capsule2 m_localShape;
};