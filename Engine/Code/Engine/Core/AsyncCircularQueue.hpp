#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include <mutex>

class AsyncCircularQueue
	//MPSC
{
private:
	struct _Header
	{
		unsigned int size  : 31;
		unsigned int ready : 1;
	};
public:
	AsyncCircularQueue(size_t size);
	~AsyncCircularQueue();

	/// /return pointer to write
	void* ReserveForPush(size_t size);
	void* BlockedReserveForPush(size_t size);
	/// /param pData buffer you get from this queue
	void  FinalizePush(void* pData);
	/// /param outSize if nothing to read, this is untouched
	void* ReserveForPop(size_t* outSize);
	void FinalizePop(void* pOut);

	size_t GetWritableSpace() const;

private:
	unsigned char* m_buffer = nullptr;
	size_t m_bufferSize = 0;
	unsigned char* m_write = nullptr;
	unsigned char* m_read = nullptr;
	std::mutex	m_lock;
};