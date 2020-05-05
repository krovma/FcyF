#include "Engine/Physics/OBBCollider2D.hpp"
#include "Engine/Physics/Rigidbody2D.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
////////////////////////////////
OBBCollider2D::OBBCollider2D(const OBB2& localShape, Rigidbody2D* rigidbody)
	:Collider2D(COLLIDER_OBB2, rigidbody), m_localShape(localShape)
{
}

////////////////////////////////
OBB2 OBBCollider2D::GetLocalShape() const
{
	return m_localShape;
}

////////////////////////////////
OBB2 OBBCollider2D::GetWorldShape() const
{
	OBB2 world = m_localShape + m_rigidbody->GetPosition();
	world.Rotate(m_rigidbody->GetRotationDegrees());
	return world;
}

////////////////////////////////
void OBBCollider2D::DebugRender(RenderContext* renderer, const Rgba& renderColor) const
{
	std::vector<Vertex_PCU> verts;
	OBB2 worldShape = GetWorldShape();
	CPUMesh mesh(RenderBufferLayout::AcquireLayoutFor<Vertex_PCU>());
	mesh.SetBrushColor(renderColor);
	mesh.AddLine2DToMesh(worldShape.GetBottomLeft(), worldShape.GetBottomRight(), 0.1f);
	mesh.AddLine2DToMesh(worldShape.GetBottomRight(), worldShape.GetTopRight(), 0.1f);
	mesh.AddLine2DToMesh(worldShape.GetTopRight(), worldShape.GetTopLeft(), 0.1f);
	mesh.AddLine2DToMesh(worldShape.GetTopLeft(), worldShape.GetBottomLeft(), 0.1f);
	GPUMesh gpuMesh(renderer);
	gpuMesh.CreateFromCPUMesh(mesh);
	renderer->BindTextureViewWithSampler(0, nullptr);
	renderer->DrawMesh(gpuMesh);
}