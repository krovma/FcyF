#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Develop/Callstack.hpp"
#include <atomic>
#include <mutex>

extern std::atomic<size_t> g_allocationCounter;

void* UntrackedAlloc(size_t size);
void  UntrackedFree(void* p);
void* TrackedAlloc(size_t size);
void  TrackedFree(void* p);
void TrackAllocation(void* p, size_t size);
void UntrackAllocation(void* p);

void* operator new(size_t size);
void operator delete(void* p);


std::string GetByteSizeString(ptrdiff_t size);

size_t GetLiveAllocationCount();
size_t GetLiveAllocationSize();
void LogLiveAllocations();

struct MemoryAllocationInfo
{
	void* m_originalPointer;
	size_t m_sizeByte;
	Callstack m_callstack;
};

template <typename T>
inline T* Alloc()
{
#if (MEM_TRACKING > MEM_TRACKING_DISABLE)
	return static_cast<T*>(TrackedAlloc(sizeof(T)));
#else
	return static_cast<T*>(UntrackedAlloc(sizeof(T)));
#endif
}

inline void Free(void* p)
{
#if (MEM_TRACKING>MEM_TRACKING_DISABLE)
	TrackedFree(p);
#else
	UntrackedFree(p);
#endif
}

struct IAllocator
{
public:
	virtual void* ialloc(size_t count) = 0;
	virtual void ifree(void* p) = 0;

	template<typename T, typename... Args>
	T* CreateObject(Args&&... args)
	{
		void* allocated = ialloc(sizeof(T));
		if (allocated) {
			new (allocated) T(args...);
		}
		return (T*)allocated;
	}

	template <typename T>
	void DestroyObject(T* p)
	{
		if (p) {
			p->~T();
			ifree(p);
		}
	}
};

template <typename T>
struct UntrackedAllocator : public IAllocator
{
	UntrackedAllocator() = default;

	template <typename U>
	UntrackedAllocator(const UntrackedAllocator<U>&) noexcept {}

	typedef T				value_type;
	typedef size_t			size_type;
	typedef T*				pointer;
	typedef const T*		const_pointer;
	typedef T&				reference;
	typedef const T&		const_reference;
	typedef std::ptrdiff_t	difference_type;

	typedef std::true_type	propagate_on_container_move_assignment;
	typedef std::true_type	is_always_equal;

	virtual void* ialloc(size_t count) override
	{
		return ::malloc(count * sizeof(value_type));
	}

	virtual void ifree(void* p) override
	{
		return ::free(p);
	}

	pointer allocate(size_type count)
	{
		return static_cast<pointer>(ialloc(count));
	}

	void deallocate(pointer p, size_type size)
	{
		UNUSED(size);
		ifree(p);
	}
};

template<typename T1, typename T2>
constexpr bool operator==(const UntrackedAllocator<T1>& a, const UntrackedAllocator<T2>& b)
{
	return true;
}

template<typename T1, typename T2>
constexpr bool operator!=(const UntrackedAllocator<T1>& a, const UntrackedAllocator<T2>& b)
{
	return false;
}

template<typename T>
struct TrackedAllocator : public IAllocator
{
	TrackedAllocator() = default; 

	template <typename U>
	TrackedAllocator(const UntrackedAllocator<U>&) noexcept {}

	typedef T				value_type;
	typedef size_t			size_type;
	typedef T* pointer;
	typedef const T* const_pointer;
	typedef T& reference;
	typedef const T& const_reference;
	typedef std::ptrdiff_t	difference_type;

	typedef std::true_type	propagate_on_container_move_assignment;
	typedef std::true_type	is_always_equal;

	virtual void* ialloc(size_t count) override
	{
		return TrackedAlloc(count * sizeof(value_type));
	}
	
	virtual void ifree(void* p) override
	{
		return TrackedFree(p);
	}
};

template<typename T>
TrackedAllocator<T>* GetTrackedAllocator()
{
	static TrackedAllocator<T>* instance = new TrackedAllocator<T>();
	return instance;
}

struct BlockAllocator : public IAllocator
{
	struct Block
	{
		Block* next;
	};
	struct Chunk
	{
		Chunk* next;
	};
public:
	bool Init(IAllocator* base, size_t blockSize, size_t alignment, unsigned int blocksPerChunk);
	bool Init(void* buffer, size_t bufferSize, size_t blockSize, size_t alignment);
	void Close();
	void* AllocBlock();
	void FreeBlock(void* block);
	Block* PopFreeBlock();
	void PushFreeBlock(Block* block);

	virtual void* ialloc(size_t count) override final
	{
		if (count * sizeof(char) <= m_blockSize) {
			return AllocBlock();
		} else {
			return nullptr;
		}
	}
	
	virtual void ifree(void* p) override final
	{
		FreeBlock(p);
		//delete p;
	}

	bool AllocChunk();

	void BreakChunk(void* chunk);
public:
	IAllocator* m_base = nullptr;

	Block* m_freeBlocks = nullptr;
	Chunk* m_chunkList = nullptr;
	size_t m_alignment = 0;
	size_t m_blockSize = 0;
	size_t m_bufferSize = 0;
	int m_blocksPerChunk = 0;

	std::mutex m_chunkLock;
	std::mutex m_blockLock;
};

BlockAllocator* GetBlockAllocator();

template<typename T>
struct ObjectBlockAllocator : public BlockAllocator
{

};