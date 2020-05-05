#pragma once
//-----------------------------------------------------------------------------------------------
#include <string>
#include <vector>
#include "EngineCommon.hpp"
#include <intrin.h>
//#define UNUSED(x) (void)(x)


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... );
const std::string Stringf( const int maxLength, const char* format, ... );
/// assume \0 at end of string
std::vector<std::string> Split(const char* cstring, char delimiter = ' ', bool multiDelimitersAsOne = true, bool reserveDelimiter = false);

size_t LoadTextFileToString(const std::string& filePath, std::string& out_string);
size_t LoadFileToBuffer(unsigned char* buffer, size_t buffer_size, const char* path);

template<typename T>
T reverse_bytes(T v)
{
	if constexpr (sizeof(T) == 1) {
		return v;
	}
	if constexpr (sizeof(T) == 2) {
		return static_cast<T>(_byteswap_ushort(static_cast<unsigned short>(v)));
	}
	if constexpr (sizeof(T) == 4) {
		return static_cast<T>(_byteswap_ulong(static_cast<unsigned long>(v)));
	}
	if constexpr (sizeof(T) == 8) {
		return static_cast<T>(_byteswap_uint64(static_cast<unsigned long long>(v)));
	}
	ERROR_AND_DIE("Not supported data type to reverse");
	//return v;
}

class buffer_reader
{
public:
	using byte = unsigned char;
	buffer_reader(byte* buffer, size_t size)
		: m_buffer(buffer)
		, m_end(buffer + size + 1)
		, m_ptr(buffer)
	{		
	}
	buffer_reader(const buffer_reader& copy) = default;
	// 	: m_buffer(copy.m_buffer)
	// 	, m_end(copy.m_end)
	// 	, m_ptr(copy.m_ptr)
	// {
	// }
	~buffer_reader() = default;
	
	template<typename T>
	T next_basic()
	{
		static_assert(std::is_scalar_v<T>, "Must be a basic type");
		constexpr size_t size_element = sizeof(T);
		GUARANTEE_OR_DIE(m_ptr + size_element < m_end, "No enough byte to read");
		T result = *(reinterpret_cast<T*>(m_ptr));
		if (m_reverse) {
			result = reverse_bytes(result);
		}
		m_ptr += size_element;
		return result;
	}

	bool next_n_byte(byte* output_buffer, size_t n)
	{
		//(m_ptr + n < m_end, "No enough byte to read");
		if (m_ptr+n > m_end) {
			return false;
		}
		if (!output_buffer) {
			m_ptr += n;
			return true;
		}
		for (size_t i = 0; i < n; ++i) {
			output_buffer[i] = *m_ptr;
			++m_ptr;
		}
		return true;
	}

	template<typename T>
	T next_user_data()
	{
		T result;
		result.set_from_buffer_reader(*this);
		return result;
	}

	void seek(size_t offset_from_beginning)
	{
		if (offset_from_beginning >= m_end - m_buffer) {
			ERROR_RECOVERABLE("Offset is too big");
		}
		m_ptr = m_buffer + offset_from_beginning;
	}

public:
	byte*		m_buffer;
	byte* const		m_end;
	byte*			m_ptr;
	bool			m_reverse = false;
};

class buffer_writer
{
public:
	using byte = unsigned char;

	void append_byte(byte v)
	{
		m_bytes.push_back(v);
	}

	template<typename T>
	void append_multi_byte(T v)
	{
		constexpr unsigned size = sizeof(T);
		static_assert(size == 2 || size == 4 || size == 8, "Unsupported type");
		T reversed = m_reverse ? reverse_bytes(v) : v;
		byte* reversed_bytes = reinterpret_cast<byte*>(&reversed);
		for (unsigned i = 0; i < size; ++i) {
			append_byte(reversed_bytes[i]);
			//reversed >>= 8;
		}
	}

	void append_byte_array(const byte* v, size_t size)
	{
		m_bytes.resize(m_bytes.size() + size, 0);
		for (unsigned i = 0; i < size; ++i) {
			append_byte(v[i]);
		}
	}

	void append_c_str(const char* v)
	{
		const char* c = v;
		while (*c != '\0') {
			append_byte(*c);
			++c;
		}
	}

	template<typename T>
	size_t append_user_data(T& data)
	{
		return data.append_to_buffer_writer(*this);
	}

	size_t get_current_offset() const
	{
		return m_bytes.size();
	}
	
	template<typename T>
	size_t overwrite_bytes(size_t pos, T v)
	{
		constexpr unsigned size = sizeof(T);
		if(pos + size >= m_bytes.size()) {
			ERROR_AND_DIE("Not enough space to overwrite");
		}
		T reversed = m_reverse ? reverse_bytes(v) : v;
		byte* reversed_bytes = reinterpret_cast<byte*>(&reversed);
		for (size_t i = pos; i < pos + size; ++i) {
			m_bytes[i] = reversed_bytes[i - pos];
		}
		return pos + size;
	}

	size_t overwrite_byte_array(size_t pos, const byte* v, size_t size)
	{
		if(pos + size >= m_bytes.size()) {
			ERROR_AND_DIE("Not enough space to overwrite");
		}
		for (size_t i = pos; i < pos + size; ++i) {
			m_bytes[i] = v[i - pos];
		}
		return pos + size;
	}
	bool m_reverse = false;
	std::vector<byte> m_bytes;
};

/*template<typename T>
T MakeFromString(const std::string& str, const T& defaultValue)
{
	T val(defaultValue);
	val.SetFromText(str.c_str());
	return val;
}*/

/*float MakeFromString(const std::string& str, const float& defaultValue)
{
	float result = defaultValue;
	result = (float)atof(str.c_str());
	return result;
}*/

