#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/FloatRange.hpp"

//////////////////////////////////////////////////////////////////////////
STATIC const FloatRange FloatRange::ZERO(0.f);

////////////////////////////////
FloatRange::FloatRange(float min, float max)
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
FloatRange::FloatRange(float minAndMax)
	: min(minAndMax)
	, max(minAndMax)
{
}

////////////////////////////////
void FloatRange::SetFromText(const char* text)
{
	std::vector<std::string> splited = Split(text, '~');
	
	if (splited.size() == 1) {
		min = (float)std::atof(splited[0].c_str());
		max = min;
	} else if (splited.size() == 2) {
		float t_min = (float)std::atof(splited[0].c_str());
		float t_max = (float)std::atof(splited[1].c_str());
		*this = FloatRange(t_min, t_max);
	} else {
		ERROR_AND_DIE(Stringf("[ERROR]%s: Cannot convert %s to FloatRange", FUNCTION, text));
		return;
	}
}

////////////////////////////////
bool FloatRange::IsInclude(float x) const
{
	return x >= min && x <= max;
}
