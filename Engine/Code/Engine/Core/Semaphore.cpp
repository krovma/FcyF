#include "Engine/Core/Semaphore.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
////////////////////////////////
Semaphore::Semaphore(int initial /*= 1*/, int max /*= 1*/)
{
	m_handle = ::CreateSemaphore(nullptr, initial, max, nullptr);
}

////////////////////////////////
Semaphore::~Semaphore()
{
	if (m_handle)
	{
		::CloseHandle(m_handle);
		m_handle = nullptr;
	}
}

////////////////////////////////
void Semaphore::BlockedAcquire()
{
	::WaitForSingleObject(m_handle, INFINITE);
}

////////////////////////////////
bool Semaphore::TryAcquire()
{
	const DWORD r = ::WaitForSingleObject(m_handle, 0);
	return r == WAIT_OBJECT_0; 
}

////////////////////////////////
void Semaphore::Release(int count /*= 1*/)
{
	::ReleaseSemaphore(m_handle, count, nullptr);
}

