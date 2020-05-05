#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
////////////////////////////////
IndexBuffer::IndexBuffer(RenderContext* renderer)
	:RenderBuffer(renderer)
{
	m_bufferUsage = RENDER_BUFFER_USAGE_INDEX;
	m_memoryUsage = GPU_MEMORY_USAGE_DYNAMIC;
}

////////////////////////////////
IndexBuffer::~IndexBuffer()
{

}

////////////////////////////////
bool IndexBuffer::Buffer(const void* data, int count)
{
	int size = count * sizeof(int);
	bool result = false;
	m_numVertices = 0;
	if (size > m_bufferSize || IsImmutable()) {
		result = RenderBuffer::Create(
			data
			, size
			, sizeof(int)
			, RENDER_BUFFER_USAGE_INDEX
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

////////////////////////////////
bool IndexBuffer::CreateImmutable(const int* data, int count)
{
	bool result = RenderBuffer::Create(
		data
		, sizeof(int) * count
		, sizeof(int)
		, RENDER_BUFFER_USAGE_INDEX
		, GPU_MEMORY_USAGE_DYNAMIC
	);
	m_numVertices = 0;
	if (result) {
		m_numVertices = count;
	}
	return result;
}