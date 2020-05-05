#pragma once
#include <set>
#include <string>

//////////////////////////////////////////////////////////////////////////
class Tag
{
public:
	/// param: tags - comma separated tags (no space)
	///			e.g: "poisoned,!sleeping"
	///			! to remove
	void SetOrRemoveTags(const std::string& tags);
	/// param: tags - comma separated tags (no space)
	///			e.g: "poisoned,!sleeping"
	///			! to check not exist
	bool HasTags(const std::string& tags) const;

	/// param: tag - single tag without prefixes
	///	behavior: do nothing if tag's already existed
	void SetTag(const std::string& tag);

	/// param: tag - single tag without prefixes
	///	behavior: do nothing if tag doesn't exist
	void RemoveTag(const std::string& tag);

	/// param: tag - single tag without prefixes
	bool HasTag(const std::string& tag) const;
private:
	std::set<std::string> m_tags;
};