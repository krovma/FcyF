#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntRange.hpp"

//////////////////////////////////////////////////////////////////////////
STATIC const IntRange IntRange::ZERO(0);
////////////////////////////////
IntRange::IntRange(int min, int max)
{
	if (min <= max) {
		this->min = min;
		this->max = max;
	} else {
		this->min = max;
		this->max = min;
	}
}

////////////////////////////////
IntRange::IntRange(int minAndMax)
	: min(minAndMax)
	, max(minAndMax)
{
}

////////////////////////////////
void IntRange::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, '~');
	
	if (splited.size() == 1) {
		min = (int)std::atoi(splited[0].c_str());
		max = min;
	} else if (splited.size() == 2) {
		int t_min = std::atoi(splited[0].c_str());
		int t_max = std::atoi(splited[1].c_str());
		*this = IntRange(t_min, t_max);
	} else {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to IntRange", FUNCTION, text));
		return;
	}
}

////////////////////////////////
bool IntRange::IsInclude(int x) const
{
	return x >= min && x <= max;
}
