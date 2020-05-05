#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/CPUMesh.hpp"

STATIC BufferAttribute Vertex_PCU::Layout[] = {
	BufferAttribute("POSITION", RBD_FLOAT3, offsetof(Vertex_PCU, m_position)),
	BufferAttribute("COLOR", RBD_RGBA, offsetof(Vertex_PCU, m_color)),
	BufferAttribute("TEXCOORD", RBD_FLOAT2, offsetof(Vertex_PCU, m_uvTexCoords)),
	BufferAttribute::END
};

////////////////////////////////
STATIC void Vertex_PCU::VertexMasterCopyProc(void* dst, const VertexMaster* src, int count)
{
	Vertex_PCU* buffer = (Vertex_PCU*)dst;
	for (int i = 0; i < count; ++i) {
		buffer[i].m_position = src[i].Position;
		buffer[i].m_color = src[i].Color;
		buffer[i].m_uvTexCoords = src[i].UV;
	}
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCU::Vertex_PCU()
{
}

//////////////////////////////////////////////////////////////////////////
Vertex_PCU::Vertex_PCU(const Vec3 &position, const Rgba &color, const Vec2 &uvTexCoords)
	: m_position(position)
	, m_color(color)
	, m_uvTexCoords(uvTexCoords)
{
}

const Vertex_PCU & Vertex_PCU::operator=(const Vertex_PCU &copyFrom)
{
	m_position = copyFrom.m_position;
	m_color = copyFrom.m_color;
	m_uvTexCoords = copyFrom.m_uvTexCoords;
	return *this;
}

