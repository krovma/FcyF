//-----------------------------------------------------------------------------------------------
// Time.cpp
//	

//-----------------------------------------------------------------------------------------------
#include "Engine/Core/Time.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Core/EngineCommon.hpp"


//-----------------------------------------------------------------------------------------------
double InitializeTime( LARGE_INTEGER& out_initialTime )
{
	LARGE_INTEGER countsPerSecond;
	QueryPerformanceFrequency( &countsPerSecond );
	QueryPerformanceCounter( &out_initialTime );
	return( 1.0 / static_cast< double >( countsPerSecond.QuadPart ) );
}

static LARGE_INTEGER initialTime;
static double secondsPerCount = InitializeTime(initialTime);

//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds()
{
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter( &currentCount );
	LONGLONG elapsedCountsSinceInitialTime = currentCount.QuadPart - initialTime.QuadPart;

	double currentSeconds = static_cast< double >( elapsedCountsSinceInitialTime ) * secondsPerCount;
	return currentSeconds;
}

////////////////////////////////
uint64 GetCurrentHPC()
{
	LARGE_INTEGER currentCount;
	QueryPerformanceCounter(&currentCount);
	return *(uint64*)(&currentCount);
}

////////////////////////////////
double HPCToSeconds(uint64 hpc)
{
	return (double)hpc * secondsPerCount;
}
