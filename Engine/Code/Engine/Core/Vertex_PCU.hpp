#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderBufferLayout.hpp"

struct Vertex_PCU
{
public:
	static BufferAttribute Layout[];
	static void VertexMasterCopyProc(void* dst, const VertexMaster* src, int count);
public:
	Vertex_PCU();
	explicit Vertex_PCU(const Vec3 &position, const Rgba &color, const Vec2 &uvTexCoords);
	const Vertex_PCU &operator=(const Vertex_PCU &copyFrom);
public:
	Vec3 m_position;
	Rgba m_color;
	Vec2 m_uvTexCoords;
};