#pragma once
#include "Engine/Physics/Collider2D.hpp"
#include "Engine/Math/OBB2.hpp"
//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
class OBBCollider2D : public Collider2D
{
public:
	OBBCollider2D(const OBB2& localShape, Rigidbody2D* rigidbody);
	OBB2 GetLocalShape() const;
	OBB2 GetWorldShape() const;

	virtual void DebugRender(RenderContext* renderer, const Rgba& renderColor) const override;
private:
	OBB2 m_localShape;
};