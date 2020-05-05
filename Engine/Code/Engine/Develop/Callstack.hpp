#pragma once
#include "Engine/Core/EngineCommon.hpp"
#define CALLSTACK_MAX_TRACE 64

class Callstack
{
public:
	void* m_trace[CALLSTACK_MAX_TRACE] = {};
	int m_depth = 0;
	int m_hash = 0;

	Callstack() = default;
	Callstack(const Callstack& copyFrom);
};

// /param frameToSkip includes GetCallstack it self
void GetCallstack(Callstack& out_callstack, int frameToSkip = 1);
std::vector<std::string> CallstackToString(const Callstack& callstack);