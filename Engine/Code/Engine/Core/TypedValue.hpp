#pragma once
/*
#include <string>
#include "Engine/Core/StringUtils.hpp"
class BaseValue
{
public:
	virtual std::string ToString() const = 0;
};

template<typename T>
class TypedValue : public BaseValue
{
public :
	TypedValue(const T& value)
		: m_value(value)
	{ }
	virtual std::string ToString() const override
	{
		return "";
	}
	T m_value;
};

class TypedParams
{
public:
	template<typename T>
	T Get(const std::string& key, const T& defaultValue)
	{
		auto found = m_values.find(key);
		if (found == m_values.end()) {
			return defaultValue;
		}
		auto v = found->second;
		TypedValue<T>* typed = dynamic_cast<TypedValue<T>*>(v);
		if(typed == nullptr) {
			std::string str = v->ToString();
			return ::MakeFromString(str, defaultValue);
		}
		return typed->m_value;
	}
	template<typename T>
	void Set(const std::string& key, const T& value)
	{
		TypedValue<T>* v = new TypedValue<T>(value);
		auto found = m_values.find(key);
		if (found != m_values.end()) {
			delete found->second;
		}
		m_values[key] = v;
	}

	void Set(const std::string& key, const char* str)
	{
		Set(key, std::string(str));
	}

	std::string Get(const std::string& key, const char* defaultValue)
	{
		return Get<std::string>(key, defaultValue);
	}

	template <typename T>
	void Set(const std::string& key, T* p)
	{
		Set<T*>(key, p);
	}

	template <typename T>
	T* Get(const std::string& key, T* defaultValue)
	{
		auto found = m_values.find(key);
		if (found == m_values.end()) {
			return defaultValue;
		}
		auto v = found->second;
		TypedValue<T*>* typed = dynamic_cast<TypedValue<T*>*>(v);
		if (typed == nullptr) {
			return defaultValue;
		}
		return typed->m_value;
	}

private:
	std::map<std::string, BaseValue*> m_values;
};
*/