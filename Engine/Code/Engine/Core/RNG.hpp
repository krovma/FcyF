#pragma once

struct FloatRange;
struct IntRange;

class RNG
{
public:
	RNG();
	RNG(int seed);
	void Init(int seed=0, int position=0);
	/// Get Int in Range [0, maxExclude)
	int GetInt(int maxExclude);
	/// Get Int in Range [minInclude, maxInclude]
	int GetIntInRange(int minInclude, int maxInclude);
	int GetIntInRange(IntRange range);
	/// Get Float in Range [0.f, maxInclude]
	float GetFloat(float maxInclude);
	/// Get Float in Range [minInclude, maxInclude]
	float GetFloatInRange(float minInclude, float maxInclude);
	float GetFloatInRange(FloatRange range);
	/// Get float in [0,1]
	float GetFloatNormed();

	/// Noise
	float GetRawNoise();

private:
	int _Rand();
	int m_currentSeed;
	int m_currentPosition;
};

extern RNG g_rng;