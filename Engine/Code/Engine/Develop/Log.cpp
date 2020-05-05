#include "Engine/Develop/Log.hpp"
#include "Engine/Develop/Callstack.hpp"
#include "Engine/Develop/Memory.hpp"
#include "Engine/Develop/DevConsole.hpp"
#include <shared_mutex>
#include <cstdio>
#include <cstdarg>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
////////////////////////////////
LogItem::~LogItem()
{
	delete callstack;
}

////////////////////////////////
size_t LogItem::GetSize() const
{
	return sizeof(LogItem)+ messageSize;
}

//////////////////////////////////////////////////////////////////////////
LogSystem* g_logSystem;
static std::shared_mutex _mapLock;
FILE* flog = nullptr;
//////////////////////////////////////////////////////////////////////////
static void LogThread()
{
	char hpc_str[30];
	fopen_s(&flog, g_logSystem->m_filename.c_str(), "wb");
	if (!flog)
	{
		ERROR_AND_DIE("Cannot create log file");
	}
	while (g_logSystem->IsRunning()) {
		g_logSystem->WaitForWork();
		LogItem* item = nullptr;
		size_t itemSize = 0;
		item = (LogItem*)g_logSystem->m_messages->ReserveForPop(&itemSize);
		while (item) {
			//sprintf_s(hpc_str, "[%I64d]", item->hpc);
			//fwrite(i)
			fwrite(item->message, 1, item->messageSize, flog);
			fwrite("\n", 1, 1, flog);

			g_logSystem->m_messages->FinalizePop(item);
			item = (LogItem*)g_logSystem->m_messages->ReserveForPop(&itemSize);

		}
	}
//	ERROR_AND_DIE("IMHERE");
	LogItem* item = nullptr;
	size_t itemSize = 0;
	item = (LogItem*)g_logSystem->m_messages->ReserveForPop(&itemSize);
	while (item) {
		//sprintf_s(hpc_str, "[%I64d]", item->hpc);
		//fwrite(i)
		fwrite(item->message, 1, item->messageSize, flog);
		fwrite("\n", 1, 1, flog);

		g_logSystem->m_messages->FinalizePop(item);
		item = (LogItem*)g_logSystem->m_messages->ReserveForPop(&itemSize);
	}

	fflush(flog);
	fclose(flog);
}

////////////////////////////////
void LogStart(const std::string& filename)
{
	g_logSystem = new LogSystem();
	g_logSystem->m_filename = filename;
	g_logSystem->m_signal = new Semaphore(0, 1);
	g_logSystem->m_logThread = std::thread(LogThread);
	g_logSystem->m_messages = new AsyncCircularQueue(2048); //2 KiB
}

////////////////////////////////
void LogStop()
{
	//g_logSystem->SignalWork();
	Log("LogSystem", "Stop logging.");
	//g_logSystem->SignalWork();
	g_logSystem->Shutdown();
	g_logSystem->SignalWork();
	g_logSystem->m_logThread.join();
}

////////////////////////////////
void Log(const char* filter, const char* fmt, ...)
{
	static constexpr size_t headerSize = sizeof(LogItem);

	// shared lock
	{
		std::shared_lock _(_mapLock);
		if (!g_logSystem->m_unfiltered &&
			g_logSystem->m_filters.find(filter) == g_logSystem->m_filters.end())
		{
			return;
		}
	}
	LogItem* newLogItem = nullptr;
	newLogItem = new LogItem();
	newLogItem->hpc = GetCurrentHPC();
	newLogItem->filter = filter;
	char msg[LOG_MAX_MESSAGE_LENGTH];
	va_list vl;
	va_start(vl, fmt);
	vsnprintf_s(msg, LOG_MAX_MESSAGE_LENGTH, fmt, vl);
	va_end(vl);
	msg[LOG_MAX_MESSAGE_LENGTH - 1] = 0;
	std::string msgString;
	if (strlen(filter) == 0) {
		msgString = msg;
	} else {
		msgString = Stringf("[%s]%s", filter, msg);
	}
	/*if (IsDebuggerAvailable()) {
		OutputDebugStringA(msgString.c_str());
	}*/
	if (g_logSystem->IsRunning() && g_theConsole)
	{
		g_theConsole->Print(msgString);
	}
	newLogItem->messageSize = msgString.size();
	size_t msgSize = headerSize + newLogItem->messageSize;
	void* buf = g_logSystem->m_messages->BlockedReserveForPush(msgSize);
	memcpy_s(buf, msgSize, newLogItem, headerSize);
	((LogItem*)buf)->message = (char*)buf + headerSize;
	memcpy_s((char*)buf + headerSize, msgSize - headerSize, msgString.c_str(), newLogItem->messageSize);
	g_logSystem->m_messages->FinalizePush(buf);
	g_logSystem->SignalWork();
	delete newLogItem;
}

////////////////////////////////
void LogFilterEnableAll()
{
	g_logSystem->m_unfiltered = true;
}

////////////////////////////////
void LogFilterDisableAll()
{
	g_logSystem->m_unfiltered = false;
}

////////////////////////////////
void LogFilterEnable(const std::string& filter)
{
	std::scoped_lock<std::shared_mutex> _(_mapLock);
	g_logSystem->m_filters.insert(filter);
}

////////////////////////////////
void LogFilterDisable(const std::string& filter)
{
	std::scoped_lock<std::shared_mutex> _(_mapLock);
	g_logSystem->m_filters.erase(filter);
}

////////////////////////////////
void LogFlush()
{
	fflush(flog);
}

////////////////////////////////
void LogSystem::WaitForWork()
{
	m_signal->BlockedAcquire();
}

////////////////////////////////
void LogSystem::SignalWork()
{
	m_signal->Release();
}

////////////////////////////////
bool LogSystem::IsRunning() const
{
	return m_running;
}

////////////////////////////////
void LogSystem::Shutdown()
{
	m_running = false;
}
