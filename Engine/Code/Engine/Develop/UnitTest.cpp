#include "Engine/Develop/UnitTest.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Develop/Callstack.hpp"
#include <cstring>
struct UnitTest
{
	const char* name = nullptr;
	const char* filter = nullptr;
	int priority;
	UnitTestCallback callback;
	inline bool operator()() { return callback(); }
};

static const int _MAX_UNIT_TESTS = 2048;

static UnitTest _allUnitTest[_MAX_UNIT_TESTS];
static int _unitTestCount = 0;

////////////////////////////////
UnitTestRegistrar::UnitTestRegistrar(const char* name, UnitTestCallback cb, const char* filter, int priority)
{
	if (_unitTestCount >= _MAX_UNIT_TESTS) {
		__debugbreak();
	} else {
		UnitTest& test = _allUnitTest[_unitTestCount];
		++_unitTestCount;

		test.name = name;
		test.filter = filter;
		test.callback = cb;
		test.priority = priority;
	}
}

////////////////////////////////
void RunUnitTest(const std::string& filter, int priorityGreaterThan)
{
	for (int i = 0; i < _unitTestCount; ++i) {
		UnitTest& test = _allUnitTest[i];
		if ((filter.empty() || (test.filter == filter)) && test.priority > priorityGreaterThan && !test()) {
			Callstack cs;
			GetCallstack(cs, 1);
			auto stringCs = CallstackToString(cs);
			std::string errorString;
			for (auto& each : stringCs) {
				errorString.append(each + "\n");
			}
			DebuggerPrintf("%s", errorString.c_str());
			ERROR_AND_DIE(errorString.c_str());
		}
	}
}
