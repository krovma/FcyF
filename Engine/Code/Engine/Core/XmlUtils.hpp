#pragma once
#include "ThirdParty/TinyXML2/tinyxml2.h"
#include <string>
//-----------------------------------------------------------------------------------------------
using XmlElement = tinyxml2::XMLElement;
using XmlDocument = tinyxml2::XMLDocument;
using XmlAttr = tinyxml2::XMLAttribute;

struct Vec3;
struct Vec2;
struct Rgba;
struct IntVec2;
struct FloatRange;
struct IntRange;
struct AABB2;
//////////////////////////////////////////////////////////////////////////

void ParseXmlFromFile(XmlElement*& out_xmlRoot, const char* path);

std::string ParseXmlAttr(const XmlElement& element, const char* attrName, const std::string& defaultValue);
std::string ParseXmlAttr(const XmlElement& element, const char* attrName, const char* defaultValue);

int ParseXmlAttr(const XmlElement& element, const char* attrName, int defaultValue);
char ParseXmlAttr(const XmlElement& element, const char* attrName, char defaultValue);
bool ParseXmlAttr(const XmlElement& element, const char* attrName, bool defaultValue);
float ParseXmlAttr(const XmlElement& element, const char* attrName, float defaultValue);
Vec2 ParseXmlAttr(const XmlElement& element, const char* attrName, const Vec2& defaultValue);
Rgba ParseXmlAttr(const XmlElement& element, const char* attrName, const Rgba& defaultValue);
FloatRange ParseXmlAttr(const XmlElement& element, const char* attrName, const FloatRange& defaultValue);
IntRange ParseXmlAttr(const XmlElement& element, const char* attrName, const IntRange& defaultValue);
IntVec2 ParseXmlAttr(const XmlElement& element, const char* attrName, const IntVec2& defaultValue);
AABB2 ParseXmlAttr(const XmlElement& element, const char* attrName, const AABB2& defaultValue);
Vec3 ParseXmlAttr(const XmlElement& element, const char* attrName, const Vec3& defaultValue);
