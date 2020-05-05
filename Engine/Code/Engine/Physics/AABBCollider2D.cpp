#include "Engine/Physics/AABBCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderContext.hpp"
////////////////////////////////
AABBCollider2D::AABBCollider2D(const AABB2& localShape, Rigidbody2D* rigidbody)
	:Collider2D(COLLIDER_AABB2, rigidbody), m_localShape(localShape)
{
}

////////////////////////////////
AABB2 AABBCollider2D::GetLocalShape() const
{
	return m_localShape;
}

////////////////////////////////
AABB2 AABBCollider2D::GetWorldShape() const
{
	return m_localShape + m_rigidbody->GetPosition();
}

////////////////////////////////
void AABBCollider2D::DebugRender(RenderContext* renderer, const Rgba& renderColor) const
{
	std::vector<Vertex_PCU> verts;
	AABB2 worldShape = GetWorldShape();
	AddVerticesOfLine2D(verts, worldShape.Min, Vec2(worldShape.Min.x, worldShape.Max.y), 0.15f, renderColor);
	AddVerticesOfLine2D(verts, Vec2(worldShape.Min.x, worldShape.Max.y), worldShape.Max, 0.15f, renderColor);
	AddVerticesOfLine2D(verts, worldShape.Max, Vec2(worldShape.Max.x, worldShape.Min.y), 0.15f, renderColor);
	AddVerticesOfLine2D(verts, Vec2(worldShape.Max.x, worldShape.Min.y), worldShape.Min, 0.15f, renderColor);
	
	Vec2 end = worldShape.GetCenter() + m_rigidbody->GetVelocity();
	AddVerticesOfLine2D(verts, worldShape.GetCenter(), end, 0.1f, Rgba::WHITE);

	renderer->BindTextureViewWithSampler(0, nullptr);
	renderer->DrawVertexArray(verts.size(), verts);
}
