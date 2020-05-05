#pragma once

#include <queue>
#include <mutex>

template<typename T>
class AsyncQueue
{
public:
	void Push(const T& obj);
	bool Pop(T* out);
	size_t Size() const;
	bool Empty() const;
	std::queue<T> m_queue;
	std::mutex m_mutex;
};

////////////////////////////////
template<typename T>
bool AsyncQueue<T>::Empty() const
{
	std::lock_guard _(m_mutex);
	return m_queue.empty();
}

////////////////////////////////
template<typename T>
size_t AsyncQueue<T>::Size() const
{
	std::lock_guard _(m_mutex);
	return m_queue.size();
}

////////////////////////////////
template<typename T>
bool AsyncQueue<T>::Pop(T* out)
{
	std::lock_guard<std::mutex> _(m_mutex);
	if (m_queue.empty()) {
		return false;
	} else {
		*out = m_queue.front();
		m_queue.pop();
		return true;
	}
}

////////////////////////////////
template<typename T>
void AsyncQueue<T>::Push(const T& obj)
{
	std::lock_guard<std::mutex> _(m_mutex);
	m_queue.push(obj);
}
