#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Develop/DevConsole.hpp"
#include "Game/EngineBuildPreferences.hpp"

////////////////////////////////
void NamedStrings::PopulateFromXmlElement(const XmlElement& xml)
{
	m_strings.clear();
	const XmlAttr* attr = xml.FirstAttribute();
	while (attr != nullptr) {
		m_strings[attr->Name()] = attr->Value();
		attr = attr->Next();
	}
}

////////////////////////////////
void NamedStrings::Set(const std::string& keyName, const std::string& value)
{
	m_strings[keyName] = value;
}

#if !defined( ENGINE_DISABLE_VIDEO )
////////////////////////////////
void NamedStrings::DebugPrintToConsole(DevConsole* console)
{
	for (auto eachString : m_strings) {
		console->Print(
			Stringf("%s=%s", eachString.first.c_str(), eachString.second.c_str())
		);
	}
}
#endif

////////////////////////////////
std::string NamedStrings::GetString(const std::string& keyName, std::string defaultValue)
{
	std::string value = defaultValue;
	auto kvpair = m_strings.find(keyName);
	if (kvpair != m_strings.end()) {
		value = kvpair->second;
	}
	return value;
}

////////////////////////////////
float NamedStrings::GetFloat(const std::string& keyName, float defaultValue)
{
	float value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value = (float) std::atof(text.c_str());
	}
	return value;
}

////////////////////////////////
int NamedStrings::GetInt(const std::string& keyName, int defaultValue)
{
	int value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value = std::atoi(text.c_str());
	}
	return value;
}

////////////////////////////////
bool NamedStrings::GetBool(const std::string& keyName, bool defaultValue)
{
	bool value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text == "true" || text == "True" || text == "TRUE" || text == "T" || text == "t" || text == "1") {
		value = true;
	} else if (text == "false" || text == "False" || text == "FALSE" || text == "F" || text == "f" || text == "0") {
		value = false;
	}
	return value;
}

////////////////////////////////
char NamedStrings::GetChar(const std::string& keyName, char defaultValue)
{
	char value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value = text[0];
	}
	return value;
}

////////////////////////////////
Rgba NamedStrings::GetRgba(const std::string& keyName, Rgba defaultValue)
{
	Rgba value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
Vec2 NamedStrings::GetVec2(const std::string& keyName, Vec2 defaultValue)
{
	Vec2 value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
Vec3 NamedStrings::GetVec3(const std::string& keyName, Vec3 defaultValue)
{
	Vec3 value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
IntVec2 NamedStrings::GetIntVec2(const std::string& keyName, IntVec2 defaultValue)
{
	IntVec2 value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
FloatRange NamedStrings::GetFloatRange(const std::string& keyName, FloatRange defaultValue)
{
	FloatRange value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
IntRange NamedStrings::GetIntRange(const std::string& keyName, IntRange defaultValue)
{
	IntRange value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
AABB2 NamedStrings::GetAABB2(const std::string& keyName, AABB2 defaultValue)
{
	AABB2 value = defaultValue;
	std::string text = GetString(keyName, "");
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}
