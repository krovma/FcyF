#pragma once
//////////////////////////////////////////////////////////////////////////
struct FloatRange
{
public:
	static const FloatRange ZERO;

	FloatRange() = default;
	///FloatRange keeps min <= max during initialization
	explicit FloatRange(float min, float max);
	explicit FloatRange(float minAndMax);
	
	/// Format
	///	min~max 
	/// ~max(min = 0)
	/// min (min = max)
	void SetFromText(const char* text);
	bool IsInclude(float x) const;

public:
	float min = 0.f;
	float max = 0.f;
};