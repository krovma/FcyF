#pragma once
#include "Engine/Renderer/RenderTypes.hpp"
#include "Engine/Renderer/RenderBufferLayout.hpp"
#include "Engine/Math/Mat4.hpp"

///////////////////////////////////////////////////////////////////////////
class RenderContext;
class CPUMesh;
struct Vertex_PCU;
class VertexBuffer;
class IndexBuffer;
//////////////////////////////////////////////////////////////////////////
class GPUMesh
{
public:
	static GPUMesh* CreateMeshFromObjFile(RenderContext* renderer, const char* path, bool invertWinding=false);
public:
	GPUMesh(RenderContext* renderer);
	~GPUMesh();

	void CreateFromCPUMesh(const CPUMesh& mesh/*, GPUMemoryUsage memoryUsage = GPU_MEMORY_USAGE_IMMUTABLE*/);
	void CopyFromCPUMesh(const CPUMesh& mesh, GPUMemoryUsage memoryUsage = GPU_MEMORY_USAGE_DYNAMIC);

	void CopyVertexPCUArray(const Vertex_PCU* vertices, size_t count);
	void CopyIndices(const int* indeces, size_t count);

	void SetDrawCall(bool usingIndexBuffer, int count);
	VertexBuffer* GetVertexBuffer() const { return m_vertexBuffer; }
	IndexBuffer* GetIndexBuffer() const { return m_indexBuffer; }
	bool IsUsingIndexBuffer() const { return m_usingIndexBuffer; }
	int GetElementCount() const { return m_count; }
	const RenderBufferLayout* GetLayout() const { return m_layout; }
	void SetLayout(const RenderBufferLayout* layout) { m_layout = layout; }

private:
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
	int m_count = 0;
	bool m_usingIndexBuffer = false;
	const RenderBufferLayout* m_layout = nullptr;
};