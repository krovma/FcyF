#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <sstream>

//-----------------------------------------------------------------------------------------------
const int STRINGF_STACK_LOCAL_TEMP_LENGTH = 2048;


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const char* format, ... )
{
	char textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, STRINGF_STACK_LOCAL_TEMP_LENGTH, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ STRINGF_STACK_LOCAL_TEMP_LENGTH - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	return std::string( textLiteral );
}


//-----------------------------------------------------------------------------------------------
const std::string Stringf( const int maxLength, const char* format, ... )
{
	char textLiteralSmall[ STRINGF_STACK_LOCAL_TEMP_LENGTH ];
	char* textLiteral = textLiteralSmall;
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		textLiteral = new char[ maxLength ];

	va_list variableArgumentList;
	va_start( variableArgumentList, format );
	vsnprintf_s( textLiteral, maxLength, _TRUNCATE, format, variableArgumentList );	
	va_end( variableArgumentList );
	textLiteral[ maxLength - 1 ] = '\0'; // In case vsnprintf overran (doesn't auto-terminate)

	std::string returnValue( textLiteral );
	if( maxLength > STRINGF_STACK_LOCAL_TEMP_LENGTH )
		delete[] textLiteral;

	return returnValue;
}

////////////////////////////////
std::vector<std::string> Split(const char* cstring, char delimiter /*= ' '*/, bool multiDelimitersAsOne /*=true*/, bool reserveDelimiter /*= false*/)
{
	std::vector<std::string> result;
	result.emplace_back("");
	int resultIndex = 0;
	for (int i = 0; cstring[i] != 0; ++i) {
		if (cstring[i] != delimiter) {
			result[resultIndex] += cstring[i];
		} else {
			if (result[resultIndex].empty()) {
				if (multiDelimitersAsOne && !reserveDelimiter) {
					continue;
				}
			}
			if (reserveDelimiter) {
				result[resultIndex] += cstring[i];
			}
			result.emplace_back("");
			++resultIndex;
		}
	}
	if (result[result.size() - 1].empty()) {
		result.pop_back();
	}
	return result;
}

////////////////////////////////
size_t LoadTextFileToString(const std::string& filePath, std::string& out_string)
{
	std::ifstream file;
	std::stringstream fileStream;
	try {
		file.open(filePath);
		fileStream << file.rdbuf();
		file.close();
	} 	catch (std::ifstream::failure& error) {
		ERROR_AND_DIE(Stringf("Failed on reading file %s\n%s\n", filePath.c_str(), error.what()).c_str());
	}
	out_string = fileStream.str();
	return out_string.length();
}

////////////////////////////////
size_t LoadFileToBuffer(unsigned char* buffer, size_t buffer_size, const char* path)
{
	FILE* fp = nullptr;
	fopen_s(&fp, path, "rb");
	if (!fp) {
		ERROR_RECOVERABLE("NO THAT FILE");
		return 0;
	}
	fseek(fp, 0, SEEK_END);
	const size_t file_size = ftell(fp);
	if (file_size > buffer_size) {
		ERROR_RECOVERABLE("Not enough buffer size");
		return 0;
	}
	rewind(fp);

	const size_t size_loaded = fread_s(buffer, buffer_size, 1, file_size, fp);
	fclose(fp);
	return size_loaded;
}


