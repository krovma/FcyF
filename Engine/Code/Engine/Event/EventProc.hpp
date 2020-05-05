#pragma once
#include "Engine/Core/TypedValue.hpp"
#include <functional>
using EventParam = NamedStrings;
using EventCallback = std::function<bool(EventParam& param)>;

class EventProc
{
public:
	EventProc(EventCallback cFuncCallback)
		: m_func(cFuncCallback.target<bool(*)(EventParam&)>())
		, m_object(nullptr)
	{
		m_callback = [=](EventParam& param) {
			return cFuncCallback(param);
		};
	}

	template<typename T>
	EventProc(T* object, bool (T::*methodCallback)(EventParam&))
	{
		m_object = object;
		m_func = *(void**)(&methodCallback);
		m_callback = [=](EventParam& param) -> bool {
			return (object->*methodCallback)(param);
		};
	}

	EventProc& operator= (const EventProc& copyFrom) = default;

	bool operator== (const EventProc& rhs) const
	{
		return (m_object == rhs.m_object) && (m_func == rhs.m_func);
	}

	bool Call(EventParam& param) const
	{
		return m_callback(param);
	}

private:
	const void* m_func = nullptr;
	void* m_object = nullptr;
	EventCallback m_callback;
};