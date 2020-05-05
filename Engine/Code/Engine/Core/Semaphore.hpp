#pragma once

class Semaphore
{
public:
	Semaphore(int initial = 1, int max = 1);
	~Semaphore();
	void BlockedAcquire(); //P
	bool TryAcquire();		// Test And P
	void Release(int count = 1); // V
private:
	void* m_handle = nullptr;
};