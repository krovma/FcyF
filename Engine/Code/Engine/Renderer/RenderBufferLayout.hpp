#pragma once
#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>
#include "Engine/Renderer/RenderTypes.hpp"
struct VertexMaster;

using VertaxMasterCallback = void(*) (void* dst, const VertexMaster* src, int count);

inline int GetSizeOfDataType(RenderBufferDataType type) { return (int)type; }

struct BufferAttribute
{
public:
	static BufferAttribute END;
public:
	BufferAttribute() = default;
	BufferAttribute(const std::string& name, RenderBufferDataType type, size_t offset);
public:
	std::string name;
	RenderBufferDataType type = RBD_NULL;
	size_t offset;

	inline bool isNull() const { return type == RBD_NULL; }
};

class RenderBufferLayout
{
public:
	static std::unordered_map<std::type_index, const RenderBufferLayout*> s_cachedLayout;
	static const RenderBufferLayout* AcquireLayoutFor(const BufferAttribute attributes[], int stride, VertaxMasterCallback vertexMasterCopyProc);
	
	template<typename T>
	static const RenderBufferLayout* AcquireLayoutFor() {
		std::type_index key(std::type_index(typeid(T)));
		auto findresult = s_cachedLayout.find(key);
		if (findresult == s_cachedLayout.end()) {
			const RenderBufferLayout* c = AcquireLayoutFor(T::Layout, sizeof(T), T::VertexMasterCopyProc);
			s_cachedLayout.insert(
				std::pair<std::type_index, const RenderBufferLayout *>(
					key, c
				)
			);
			return c;
		}
		return findresult->second;
	}

public:
	RenderBufferLayout()=default;
	RenderBufferLayout(const BufferAttribute attributes[]);
	const BufferAttribute& operator[](int index) const { return m_attributes[index]; }
private:
	std::vector<BufferAttribute> m_attributes;
	int m_stride=0;
	VertaxMasterCallback m_vertexMasterCopyProc=nullptr;

public:

	int GetAttributeCount() const { return (int)m_attributes.size(); }
	int GetStride() const { return m_stride; }
	inline void CopyFromVertexMaster(void* dst, const VertexMaster* src, int count) const
	{
		m_vertexMasterCopyProc(dst, src, count);
	}
};