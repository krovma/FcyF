#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba.hpp"
////////////////////////////////
void ParseXmlFromFile(XmlElement*& out_xmlRoot, const char* path)
{
	XmlDocument* doc;
	doc = new XmlDocument;
	if (doc->LoadFile(path) != 0) {
		ERROR_AND_DIE(Stringf("[Error]%s: Failed to Load %s\n Check Run/Data settings", FUNCTION, path));
	}
	out_xmlRoot = doc->RootElement();
}

////////////////////////////////
std::string ParseXmlAttr(const XmlElement& element, const char* attrName, const std::string& defaultValue)
{
	std::string value = defaultValue;
	const XmlAttr* attr = element.FindAttribute(attrName);
	if (attr) {
		value = attr->Value();
	}
	return value;
}

////////////////////////////////
int ParseXmlAttr(const XmlElement& element, const char* attrName, int defaultValue)
{
	int value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value = std::atoi(text.c_str());
	}
	return value;
}

////////////////////////////////
char ParseXmlAttr(const XmlElement& element, const char* attrName, char defaultValue)
{
	char value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value = text[0];
	}
	return value;
}

////////////////////////////////
bool ParseXmlAttr(const XmlElement& element, const char* attrName, bool defaultValue)
{
	bool value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text == "true" || text == "True" || text == "TRUE" || text == "T" || text == "t" || text == "1") {
		value = true;
	} else if (text == "false" || text == "False" || text == "FALSE" || text == "F" || text == "f" || text == "0") {
		value = false;
	}
	return value;
}

////////////////////////////////
float ParseXmlAttr(const XmlElement& element, const char* attrName, float defaultValue)
{
	float value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value = (float) std::atof(text.c_str());
	}
	return value;
}

////////////////////////////////
Vec2 ParseXmlAttr(const XmlElement& element, const char* attrName, const Vec2& defaultValue)
{
	Vec2 value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
Vec3 ParseXmlAttr(const XmlElement& element, const char* attrName, const Vec3& defaultValue)
{
	Vec3 value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
Rgba ParseXmlAttr(const XmlElement& element, const char* attrName, const Rgba& defaultValue)
{
	Rgba value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
FloatRange ParseXmlAttr(const XmlElement& element, const char* attrName, const FloatRange& defaultValue)
{
	FloatRange value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
IntRange ParseXmlAttr(const XmlElement& element, const char* attrName, const IntRange& defaultValue)
{
	IntRange value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
IntVec2 ParseXmlAttr(const XmlElement& element, const char* attrName, const IntVec2& defaultValue)
{
	IntVec2 value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
AABB2 ParseXmlAttr(const XmlElement& element, const char* attrName, const AABB2& defaultValue)
{
	AABB2 value = defaultValue;
	std::string text = ParseXmlAttr(element, attrName, std::string(""));
	if (text != "") {
		value.SetFromText(text.c_str());
	}
	return value;
}

////////////////////////////////
std::string ParseXmlAttr(const XmlElement& element, const char* attrName, const char* defaultValue)
{
	std::string value(defaultValue);
	const XmlAttr* attr = element.FindAttribute(attrName);
	if (attr) {
		value = attr->Value();
	}
	return value;
}
