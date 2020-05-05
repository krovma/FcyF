#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/RenderCommon.hpp"
#include "Engine/Core/EngineCommon.hpp"
////////////////////////////////
RenderBuffer::RenderBuffer(RenderContext* renderer)
	:m_renderer(renderer)
{

}

////////////////////////////////
RenderBuffer::~RenderBuffer()
{
	//DebuggerPrintf("Render buffer Decon rinvoke %x\n", this);
	DX_SAFE_RELEASE(m_handle);
}

////////////////////////////////
bool RenderBuffer::Create(const void* initialData, int bufferSize, int elementSize, RenderBufferUsage bufferUsage, GPUMemoryUsage memoryUsage)
{
	if (bufferSize == 0) {
		return false;
	}
	DX_SAFE_RELEASE(m_handle);
	GUARANTEE_OR_DIE(!IsImmutable(), "Create Immutable buffer from CreateImmutable\n");
	if (initialData==nullptr) {
		return false;
	}

	D3D11_BUFFER_DESC desc;
	memset(&desc, 0, sizeof(desc));
	desc.ByteWidth = (unsigned int)bufferSize;
	desc.StructureByteStride = (unsigned int)elementSize;
	desc.Usage = GetD3DUsageFromGPUMemoryUsage(memoryUsage);
	desc.BindFlags = GetD3DBufferUsageFromRenderBufferUsage(bufferUsage);
	if (memoryUsage == GPU_MEMORY_USAGE_DYNAMIC) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	} else if (memoryUsage == GPU_MEMORY_USAGE_STAGING) {
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}
	
	D3D11_SUBRESOURCE_DATA data;
	memset(&data, 0, sizeof(data));
	data.pSysMem = initialData;
	ID3D11Device* device = m_renderer->GetDevice();
	HRESULT hr = device->CreateBuffer(&desc, &data, &m_handle);

	if (SUCCEEDED(hr)) {
		m_bufferUsage = bufferUsage;
		m_memoryUsage = memoryUsage;
		m_bufferSize = bufferSize;
		m_elementSize = elementSize;
		return true;
	} else {
		ERROR_AND_DIE("Failed on create render buffer");
	}
	//return false;
}

////////////////////////////////
bool RenderBuffer::Buffer(const void* data, int size)
{
	if (size == 0) {
		return false;
	}
	GUARANTEE_OR_DIE(!IsImmutable(), "Can not buffer an inmmutable buffer\n");
	GUARANTEE_OR_DIE((m_bufferSize >= size),"Buffer size not enough\n");
	ID3D11DeviceContext *context = m_renderer->GetContext();
	D3D11_MAPPED_SUBRESOURCE resource;
	HRESULT hr = context->Map(m_handle, 0, D3D11_MAP_WRITE_DISCARD, 0u, &resource);
	if (SUCCEEDED(hr)) {
		memcpy(resource.pData, data, size);
		context->Unmap(m_handle, 0);
		return true;
	}
	return false;
}

////////////////////////////////
D3D11_USAGE GetD3DUsageFromGPUMemoryUsage(GPUMemoryUsage usage)
{
	switch (usage) {
	case GPU_MEMORY_USAGE_GPU: return D3D11_USAGE_DEFAULT;
	case GPU_MEMORY_USAGE_IMMUTABLE: return D3D11_USAGE_IMMUTABLE;
	case GPU_MEMORY_USAGE_DYNAMIC: return D3D11_USAGE_DYNAMIC;
	case GPU_MEMORY_USAGE_STAGING: return D3D11_USAGE_STAGING;
	default: return D3D11_USAGE_DEFAULT;
	}
}

////////////////////////////////
unsigned int GetD3DBufferUsageFromRenderBufferUsage(RenderBufferUsage usage)
{
	unsigned int bufferUsage = 0u
		| ((usage & RENDER_BUFFER_USAGE_VERTEX) ? D3D11_BIND_VERTEX_BUFFER : 0u)
		| ((usage & RENDER_BUFFER_USAGE_INDEX) ? D3D11_BIND_INDEX_BUFFER : 0u)
		| ((usage & RENDER_BUFFER_USAGE_CONSTANT) ? D3D11_BIND_CONSTANT_BUFFER : 0u)
		;
	return bufferUsage;
}
