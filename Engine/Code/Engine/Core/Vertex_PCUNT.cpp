#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCUNT.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

STATIC BufferAttribute Vertex_PCUNT::Layout[] = {
	BufferAttribute("POSITION", RBD_FLOAT3, offsetof(Vertex_PCUNT, m_position)),
	BufferAttribute("COLOR", RBD_RGBA, offsetof(Vertex_PCUNT, m_color)),
	BufferAttribute("TEXCOORD", RBD_FLOAT2, offsetof(Vertex_PCUNT, m_uvTexCoords)),
	BufferAttribute("NORMAL", RBD_FLOAT3, offsetof(Vertex_PCUNT, m_normal)),
	BufferAttribute("TANGENT", RBD_FLOAT3, offsetof(Vertex_PCUNT, m_tangent)),
	BufferAttribute::END
};

////////////////////////////////
STATIC void Vertex_PCUNT::VertexMasterCopyProc(void* dst, const VertexMaster* src, int count)
{
	Vertex_PCUNT* buffer = (Vertex_PCUNT*)dst;
	for (int i = 0; i < count; ++i) {
		buffer[i].m_position = src[i].Position;
		buffer[i].m_color = src[i].Color;
		buffer[i].m_uvTexCoords = src[i].UV;
		buffer[i].m_normal = src[i].Normal;
		buffer[i].m_tangent = src[i].Tangent;
	}
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCUNT::Vertex_PCUNT()
{
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCUNT::Vertex_PCUNT(const Vec3 &position, const Rgba &color, const Vec2 &uvTexCoords, const Vec3& normal, const Vec3& tangent)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
	, m_normal(normal)
	, m_tangent(tangent)
{
}

const Vertex_PCUNT & Vertex_PCUNT::operator=(const Vertex_PCUNT &copyFrom)
{
	m_position = copyFrom.m_position;
	m_color = copyFrom.m_color;
	m_uvTexCoords = copyFrom.m_uvTexCoords;
	m_normal = copyFrom.m_normal;
	m_tangent = copyFrom.m_tangent;
	return *this;
}

