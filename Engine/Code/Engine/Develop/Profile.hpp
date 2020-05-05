#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"
#include <thread>
using uint64 = unsigned long long int;
constexpr size_t PROFILER_MAX_RECORD = 1024;
constexpr size_t PROFILER_LABEL_SIZE = 64;
struct ScopeProfiler
{
	//uint64 m_hpc;
	//const char* m_scopeName;
	ScopeProfiler(const char* scopeName);
	~ScopeProfiler();
private:
	char _EMPTY_ = 0;
};

#define ___CONCAT___(a, b) a##b
#define ___CONCAT(a, b) ___CONCAT___(a, b)
#define __PROFILE_SCOPE_IMPL(scope_name, line)\
	ScopeProfiler ___CONCAT(__scope_profiler_, line)(scope_name);
#define PROFILE_SCOPE(scope_name) __PROFILE_SCOPE_IMPL(scope_name, __LINE__)
#define PROFILED_FUNCTION PROFILE_SCOPE(__FUNCTION__);
struct ProfilerNode
{
	ProfilerNode* m_parent = nullptr;
	ProfilerNode* m_firstChild = nullptr;
	ProfilerNode* m_nextSibling = nullptr;

	char label[PROFILER_LABEL_SIZE];
	uint64 beginHPC = 0;
	uint64 endHPC;
	long int refCount = 0;

	unsigned int allocs = 0;
	unsigned int frees = 0;
	ptrdiff_t deltaByte = 0;


	void AddChild(ProfilerNode* child);
	ProfilerNode* GetParent() const;
	void SetParent(ProfilerNode* parent);
	ProfilerNode* GetLastSibling() const;
	ProfilerNode* GetNextSibling() const;
	void SetNextSibling(ProfilerNode* next);

	double GetTimeMicroSecond() const;
};

struct ProfilerTreeInfo
{
	std::thread::id threadID;
	ProfilerTreeInfo() = default;
	ProfilerTreeInfo(std::thread::id&& id)
		:threadID(id)
	{
	}
};

void ProfileInit();
int GetTotalProfiledFrames();
void ProfileReleaseTree(ProfilerNode* node);
void ProfilePush(const char* tag);
void ProfilePop();
void ProfileFreeTree(ProfilerNode* root);
void ProfilePause();
void ProfileResume();
ProfilerNode* RequireReferenceOfProfileTree(std::thread::id threadID, int nFromBack=0);
void ShowTreeView(ProfilerNode* profileTree, bool sortBySelf = true);
void ShowFlatView(ProfilerNode* profileTree, bool sortBySelf = true);
std::vector<float> GetFrameTimeList();
