#include "Engine/Core/RNG.hpp"
#include <ctime>
#include "Engine/Math/IntRange.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/RawNoise.hpp"
//////////////////////////////////////////////////////////////////////////
RNG g_rng;
//STATIC const int RNG::RAND_MAX = 65535;

RNG::RNG()
{
	Init(0, 0);
}

////////////////////////////////
RNG::RNG(int seed)
{
	Init(seed, 0);
}

//////////////////////////////////////////////////////////////////////////
void RNG::Init(int seed, int position)
{
	m_currentSeed = seed;
	m_currentPosition = position;
}

//////////////////////////////////////////////////////////////////////////
int RNG::GetInt(int maxExclude)
{
	return _Rand() % maxExclude;
}

int RNG::GetIntInRange(int minInclude, int maxInclude)
{
	int range = maxInclude - minInclude + 1;
	return _Rand() % range + minInclude;
}

////////////////////////////////
int RNG::GetIntInRange(IntRange range)
{
	return GetIntInRange(range.min, range.max);
}

float RNG::GetFloat(float maxInclude)
{
	return GetFloatNormed() * maxInclude;
}

float RNG::GetFloatInRange(float minInclude, float maxInclude)
{
	return GetFloatNormed() * (maxInclude - minInclude) + minInclude;
}

////////////////////////////////
float RNG::GetFloatInRange(FloatRange range)
{
	return GetFloatInRange(range.min, range.max);
}

float RNG::GetFloatNormed()
{
	return Get1dNoiseZeroToOne(m_currentPosition++, m_currentSeed);
}

////////////////////////////////
float RNG::GetRawNoise()
{
	return Get1dNoiseNegOneToOne(m_currentPosition++, m_currentSeed);
}

////////////////////////////////
int RNG::_Rand()
{
	return abs((int)Get1dNoiseUint(m_currentPosition++, m_currentSeed));
}
