#pragma once
#include "Engine/Physics/Collider2D.hpp"
struct Rgba;
class DiskCollider2D : public Collider2D
{
public:
	DiskCollider2D(float radius, Rigidbody2D* rigidbody);
	float GetRadius() const;
	virtual void DebugRender(RenderContext* renderer, const Rgba& renderColor) const override;
private:
	float m_radius;
};