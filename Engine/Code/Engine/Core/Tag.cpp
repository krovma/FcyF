#include "Engine/Core/Tag.hpp"
#include "Engine/Core/EngineCommon.hpp"

////////////////////////////////
void Tag::SetOrRemoveTags(const std::string& tags)
{
	std::vector<std::string> tagList = Split(tags.c_str(), ',');
	for (auto& eachTag : tagList) {
		if (eachTag.length() > 0) {
			if (eachTag[0] == '!') {
				std::string tagName = std::string(eachTag.c_str() + 1);
				if (tagName.length() > 0) {
					RemoveTag(tagName);
				}
			} else {
				SetTag(eachTag);
			}
		}
	}
}

////////////////////////////////
bool Tag::HasTags(const std::string& tags) const
{
	std::vector<std::string> tagList = Split(tags.c_str(), ',');
	for (auto& eachTag : tagList) {
		bool result = true;
		if (eachTag.length() > 0) {
			if (eachTag[0] == '!') {
				std::string tagName = std::string(eachTag.c_str() + 1);
				if (tagName.length() > 0) {
					result = !HasTag(tagName);
				}
			} else {
				result = HasTag(eachTag);
			}
			if (!result) {
				return false;
			}
		}
	}
	return true;
}

////////////////////////////////
void Tag::SetTag(const std::string& tag)
{
	m_tags.insert(tag);
}

////////////////////////////////
void Tag::RemoveTag(const std::string& tag)
{
	m_tags.erase(tag);
}

////////////////////////////////
bool Tag::HasTag(const std::string& tag) const
{
	if (m_tags.find(tag) != m_tags.end()) {
		return true;
	}
	return false;
}
