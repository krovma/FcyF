#pragma once
#include <string>
#include <map>
#include "Engine/Core/XmlUtils.hpp"
//////////////////////////////////////////////////////////////////////////
class DevConsole;
//////////////////////////////////////////////////////////////////////////
class NamedStrings
{
public:
	void PopulateFromXmlElement(const XmlElement& xml);

	void Set(const std::string& keyName, const std::string& value);
	void DebugPrintToConsole(DevConsole* console);

	std::string GetString(const std::string& keyName, std::string defaultValue);
	float	GetFloat(const std::string& keyName, float defaultValue);
	int		GetInt(const std::string& keyName, int defaultValue);
	bool	GetBool (const std::string& keyName, bool defaultValue);
	char	GetChar(const std::string& keyName, char defaultValue);
	Rgba	GetRgba(const std::string& keyName, Rgba defaultValue);
	Vec2	GetVec2(const std::string& keyName, Vec2 defaultValue);
	Vec3	GetVec3(const std::string& keyName, Vec3 defaultValue);
	IntVec2 GetIntVec2(const std::string& keyName, IntVec2 defaultValue);
	FloatRange	GetFloatRange(const std::string& keyName, FloatRange defaultValue);
	IntRange	GetIntRange(const std::string& keyName, IntRange defaultValue);
	AABB2		GetAABB2(const std::string& keyName, AABB2 defaultValue);


private:
	std::map<std::string, std::string> m_strings;
};