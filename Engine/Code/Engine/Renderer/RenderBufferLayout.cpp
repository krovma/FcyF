#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderBufferLayout.hpp"

STATIC std::unordered_map<std::type_index, const RenderBufferLayout*> RenderBufferLayout::s_cachedLayout;
////////////////////////////////
const RenderBufferLayout* RenderBufferLayout::AcquireLayoutFor(const BufferAttribute attributes[], int stride, VertaxMasterCallback vertexMasterCopyProc)
{
	RenderBufferLayout* created = new RenderBufferLayout(attributes);
	created->m_stride = stride;
	created->m_vertexMasterCopyProc = vertexMasterCopyProc;
	return created;
}

////////////////////////////////
RenderBufferLayout::RenderBufferLayout(const BufferAttribute attributes[])
{
	m_stride = 0;
	for (auto p = attributes; /*Condition inside*/; ++p) {
		if (p->isNull()) {
			break;
		}
		m_attributes.push_back(*p);
		m_stride += GetSizeOfDataType(p->type);
	}
}

BufferAttribute BufferAttribute::END = BufferAttribute();

////////////////////////////////
BufferAttribute::BufferAttribute(const std::string& name, RenderBufferDataType type, size_t offset)
	:name(name), type(type), offset(offset)
{
}
