#include "Engine/Core/AsyncCircularQueue.hpp"

////////////////////////////////
AsyncCircularQueue::AsyncCircularQueue(size_t size)
	: m_bufferSize(size)
{
	m_buffer = new unsigned char[size];
	m_write = m_buffer;
	m_read = m_buffer;
}

////////////////////////////////
AsyncCircularQueue::~AsyncCircularQueue()
{
	delete[] m_buffer;
}

////////////////////////////////
size_t AsyncCircularQueue::GetWritableSpace() const
{
	std::ptrdiff_t diff = m_write - m_read;
	if (diff >= 0) {
		return m_bufferSize - diff;
	} else {
		// diff < 0
		return (size_t)-diff;
	}
}

////////////////////////////////
void* AsyncCircularQueue::ReserveForPush(size_t size)
{
	static constexpr size_t headerSize = sizeof(_Header);
	size_t sizeRemain = GetWritableSpace();
	size_t sizeOfBlockAndNextHeader = size + headerSize * 2;

	std::scoped_lock _(m_lock);

	if (sizeRemain < sizeOfBlockAndNextHeader) {
		return nullptr;
	}
	unsigned char* nextHead = m_write + sizeOfBlockAndNextHeader;
	if (nextHead > m_buffer + m_bufferSize) {
		//Create a wrap to head
		_Header* skip = (_Header*)m_write;
		skip->size = 0; //skip
		skip->ready = 1;
		m_write = m_buffer;
		sizeRemain = GetWritableSpace();
	}
	if (sizeRemain < sizeOfBlockAndNextHeader) {
		return nullptr;
	}
	_Header* header = (_Header*)m_write;
	header->size = (unsigned int)size;
	header->ready = 0;
	m_write += sizeOfBlockAndNextHeader;
	
	return header + 1;
}

////////////////////////////////
void* AsyncCircularQueue::BlockedReserveForPush(size_t size)
{
	void* buf = ReserveForPush(size);
	while (!buf) {

		std::this_thread::yield();
		buf = ReserveForPush(size);
	}
	return buf;
}

////////////////////////////////
void AsyncCircularQueue::FinalizePush(void* pData)
{
	_Header* head = ((_Header*)pData) - 1;
	head->ready = 1;
}

////////////////////////////////
void* AsyncCircularQueue::ReserveForPop(size_t* outSize)
{
	std::scoped_lock _(m_lock);
	while (true) {
		if (m_read == m_write) {
			return nullptr;
		}
		_Header* head = (_Header*)m_read;
		if (head->ready) {
			if (head->size == 0) {
				m_read = m_buffer;
				continue;
			}
			*outSize = head->size;
			return head + 1;
		} else {
			return nullptr;
		}
	}
}

////////////////////////////////
void AsyncCircularQueue::FinalizePop(void* pOut)
{
	static constexpr size_t headerSize = sizeof(_Header);
	std::scoped_lock _(m_lock);
	_Header* header = (_Header*)pOut - 1;
	GUARANTEE_OR_DIE((void*)m_read == header, "Circular Queue read pointer not matched");
	m_read = (unsigned char*)((size_t)header + headerSize * 2 + header->size);
}
