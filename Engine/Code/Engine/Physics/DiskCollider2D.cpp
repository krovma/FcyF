#include "Engine/Physics/DiskCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderContext.hpp"
////////////////////////////////
DiskCollider2D::DiskCollider2D(float radius, Rigidbody2D* rigidbody)
	:Collider2D(COLLIDER_DISK2D, rigidbody),m_radius(radius)
{

}

////////////////////////////////
float DiskCollider2D::GetRadius() const
{
	return m_radius;
}

////////////////////////////////
void DiskCollider2D::DebugRender(RenderContext* renderer, const Rgba& renderColor) const
{
	std::vector<Vertex_PCU> verts;
	AddVerticesOfRing2D(verts, m_rigidbody->GetPosition(), m_radius, 0.1f, renderColor);
	Vec2 end = m_rigidbody->GetPosition() + m_rigidbody->GetVelocity();
	AddVerticesOfLine2D(verts, m_rigidbody->GetPosition(), end, 0.1f, Rgba::WHITE);
	renderer->BindTextureViewWithSampler(0, nullptr);
	renderer->DrawVertexArray(verts.size(), verts);
}
