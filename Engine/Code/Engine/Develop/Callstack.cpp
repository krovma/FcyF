#include "Engine/Develop/Callstack.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include "Engine/Develop/Memory.hpp"
#include <DbgHelp.h>
#pragma comment(lib, "DbgHelp.lib")

////////////////////////////////
void GetCallstack(Callstack& out_callstack, int frameToSkip /*= 1*/)
{
	
	out_callstack.m_depth = CaptureStackBackTrace(frameToSkip, CALLSTACK_MAX_TRACE, out_callstack.m_trace, (PDWORD)&out_callstack.m_hash);
// 	for (int i = 0; i < nStack; ++i) {
// 		out_callstack.m_trace[i] = pStack[i];
// 	}
// 	out_callstack.m_depth = nStack;
}

////////////////////////////////
std::vector<std::string> CallstackToString(const Callstack& callstack)
{
	std::vector<std::string> xs;
	HANDLE currentProcess = GetCurrentProcess();
	SymInitialize(currentProcess, nullptr, true);

	for (int i = 0; i < callstack.m_depth; ++i) {
		IMAGEHLP_LINE64 line;
		SymSetOptions(SYMOPT_LOAD_LINES);
		char buf[sizeof(SYMBOL_INFO) + 255 * sizeof(char)];
		SYMBOL_INFO* sym = (SYMBOL_INFO*)buf;
		sym->SizeOfStruct = sizeof(SYMBOL_INFO);
		sym->MaxNameLen = 256;

		DWORD64 disp;
		DWORD displine;
		SymFromAddr(currentProcess, (DWORD64)callstack.m_trace[i], &disp, sym);

		SymGetLineFromAddr64(currentProcess, (DWORD64)callstack.m_trace[i], &displine, &line);
		/*
		if (!success) {
			DWORD errorno = GetLastError();
			char buf[256];
			FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorno, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				buf, (sizeof(buf) / sizeof(char)), NULL);
			ERROR_AND_DIE(buf);
		}*/
		xs.push_back(Stringf("%s(%d, %d): %s(%x)",line.FileName, line.LineNumber, displine ,sym->Name, line.Address));
	}

	SymCleanup(currentProcess);
	return xs;
}

////////////////////////////////
Callstack::Callstack(const Callstack& copyFrom)
	:m_depth(copyFrom.m_depth)
{
	for (int i = 0; i < CALLSTACK_MAX_TRACE; ++i)
	{
		m_trace[i] = copyFrom.m_trace[i];
	}
}
