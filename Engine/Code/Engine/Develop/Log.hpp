#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Semaphore.hpp"
//#include "Engine/Core/AsyncQueue.hpp"
#include "Engine/Core/AsyncCircularQueue.hpp"
#include "Engine/Core/Time.hpp"
#include <thread>
#include <set>

class Callstack;
inline constexpr int LOG_MAX_MESSAGE_LENGTH = 2048;
struct LogItem
{
	uint64 hpc = 0;
	const char* filter = nullptr;
	Callstack* callstack = nullptr;
	size_t messageSize = 0;
	char* message = nullptr; //

	~LogItem();
	size_t GetSize() const;
};

struct LogSystem
{
public:
	void WaitForWork();
	void SignalWork();
	bool IsRunning() const;
	void Shutdown();

	std::thread m_logThread;
	std::string m_filename;
	Semaphore* m_signal = nullptr;
	//AsyncQueue<LogItem*> m_messages;
	AsyncCircularQueue* m_messages;
	bool m_running = true;
	std::set<std::string> m_filters;
	bool m_unfiltered = true;
};

void LogStart(const std::string& filename);
void LogStop();
void Log(const char* filter, const char* fmt, ...);
//void LogWithCallstack(const char* filter, const char* fmt, ...);

void LogFilterEnableAll();
void LogFilterDisableAll();
void LogFilterEnable(const std::string& filter);
void LogFilterDisable(const std::string& filter);
void LogFlush();