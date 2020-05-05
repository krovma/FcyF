//-----------------------------------------------------------------------------------------------
// Time.hpp
//
#pragma once


//-----------------------------------------------------------------------------------------------
double GetCurrentTimeSeconds();

using uint64 = unsigned long long int;

uint64 GetCurrentHPC();
double HPCToSeconds(uint64 hpc);