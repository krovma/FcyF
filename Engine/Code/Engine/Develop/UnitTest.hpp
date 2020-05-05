#pragma once
#include <string>

#define CONFIRM(e) 	if (!(e)) { return false; }

using UnitTestCallback = bool(*)();
struct UnitTestRegistrar
{
	UnitTestRegistrar(const char* name, UnitTestCallback cb, const char* filter, int priority);
};

void RunUnitTest(const std::string& filter, int priorityGreaterThan=-1);

#define UNIT_TEST(name, filter, priority) \
	static bool ___unit_test___##name();		\
	static UnitTestRegistrar ___unit_test_obj___##name(#name, ___unit_test___##name, filter, priority);		\
	static bool ___unit_test___##name()
#define ALL_UNIT_TEST "" // DON'T CHANGE IT
