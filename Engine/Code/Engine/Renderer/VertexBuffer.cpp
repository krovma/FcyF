#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBufferLayout.hpp"
////////////////////////////////
VertexBuffer::VertexBuffer(RenderContext* renderer)
	:RenderBuffer(renderer)
{
	m_memoryUsage = GPU_MEMORY_USAGE_DYNAMIC;
	//#Todo: change to GPU_MEMORY_USAGE_IMMUTABLE;
	m_bufferUsage = RENDER_BUFFER_USAGE_VERTEX;
}
////////////////////////////////
VertexBuffer::~VertexBuffer()
{
	//Do nothing;
	//DebuggerPrintf("VBuffer Decon rinvoke %x\n", this);
}

////////////////////////////////
bool VertexBuffer::CreateImmutable(const void* data, int count, const RenderBufferLayout* layout)
{
	bool result = RenderBuffer::Create(
		data
		, layout->GetStride() * count
		, layout->GetStride()
		, RENDER_BUFFER_USAGE_VERTEX
		, GPU_MEMORY_USAGE_DYNAMIC
	);
	m_numVertices = 0;
	if (result) {
		m_numVertices = count;
		m_layout = layout;
	}
	return result;
}

////////////////////////////////
bool VertexBuffer::Buffer(const void* data, int count)
{
	int size = count * m_layout->GetStride();
	bool result = false;
	m_numVertices = 0;
	if (size > m_bufferSize || IsImmutable()) {
		result = RenderBuffer::Create(
			data
			, size
			, m_layout->GetStride()
			, RENDER_BUFFER_USAGE_VERTEX
			, GPU_MEMORY_USAGE_DYNAMIC
		);
		if (result) {
			m_numVertices = count;
		}
	} else {
		result = RenderBuffer::Buffer(data, size);
		if (result) {
			m_numVertices = count;
		}
	}
	return result;
}
