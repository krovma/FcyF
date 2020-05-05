#pragma once
//////////////////////////////////////////////////////////////////////////
struct IntRange
{
public:
	static const IntRange ZERO;

	IntRange() = default;
	///IntRange keeps min <= max during initialization
	explicit IntRange(int min, int max);
	explicit IntRange(int minAndMax);
	
	/// Format
	///	min~max 
	/// ~max(min = 0)
	/// min (min = max)
	void SetFromText(const char* text);
	bool IsInclude(int x) const;

public:
	int min = 0;
	int max = 0;
};