#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/RenderBufferLayout.hpp"

struct Vertex_PCUNT
{
public:
	static BufferAttribute Layout[];
	static void VertexMasterCopyProc(void* dst, const VertexMaster* src, int count);
public:
	Vertex_PCUNT();
	explicit Vertex_PCUNT(const Vec3 &position, const Rgba &color, const Vec2 &uvTexCoords, const Vec3 &normal, const Vec3& tanget);
	const Vertex_PCUNT &operator=(const Vertex_PCUNT &copyFrom);
public:
	Vec3 m_position;
	Rgba m_color;
	Vec2 m_uvTexCoords;
	Vec3 m_normal = Vec3(0, 0, 1);
	Vec3 m_tangent = Vec3(1, 0, 0);
};