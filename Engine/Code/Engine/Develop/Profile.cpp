#include "Engine/Develop/Profile.hpp"
#include "Engine/Develop/Log.hpp"
#include "Engine/Develop/Memory.hpp"
#include "Game/EngineBuildPreferences.hpp"
#include <shared_mutex>
#include <cstring>
#include <algorithm>
#include <queue>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
//////////////////////////////////////////////////////////////////////////
static thread_local ProfilerNode* t_activeNode = nullptr;
static thread_local bool t_pause = false;
static thread_local int t_depth = 0;
static std::vector<std::pair<ProfilerTreeInfo, ProfilerNode*>> g_profileHistory;
static std::shared_mutex g_historyLock;
////////////////////////////////
ScopeProfiler::ScopeProfiler(const char* scopeName)
{
	ProfilePush(scopeName);
}

////////////////////////////////
ScopeProfiler::~ScopeProfiler()
{
	ProfilePop();
}

////////////////////////////////
void ProfilerNode::AddChild(ProfilerNode* child)
{
	if (child->GetParent() != this)
	{
		child->SetParent(this);
		if (m_firstChild == nullptr) {
			m_firstChild = child;
		}
		ProfilerNode* p = m_firstChild->GetLastSibling();
		p->SetNextSibling(child);
	}

}

////////////////////////////////
ProfilerNode* ProfilerNode::GetParent() const
{
	return m_parent;
}

////////////////////////////////
void ProfilerNode::SetParent(ProfilerNode* parent)
{
	m_parent = parent;
}

/////////////////////////////////////
ProfilerNode* ProfilerNode::GetLastSibling() const
{
	const ProfilerNode* p = this;
	const ProfilerNode* q = p->GetNextSibling();
	while (q) {
		p = q;
		q = p->GetNextSibling();
	}
	return const_cast<ProfilerNode*>(p);
}

////////////////////////////////
ProfilerNode* ProfilerNode::GetNextSibling() const
{
	return m_nextSibling;
}

////////////////////////////////
void ProfilerNode::SetNextSibling(ProfilerNode* next)
{
	if (next != this) {
		m_nextSibling = next;
	}
}

////////////////////////////////
double ProfilerNode::GetTimeMicroSecond() const
{
	return HPCToSeconds(endHPC - beginHPC) * 1000000;
}

//////////////////////////////////////////////////////////////////////////
static ProfilerNode* NewNode()
{
	//ProfilerNode* created = new ProfilerNode();
	ProfilerNode* created = (ProfilerNode*) GetBlockAllocator()->ialloc(sizeof(ProfilerNode));
	created = new(created) ProfilerNode();
	created->refCount = 1;
	return created;
}

//////////////////////////////////////////////////////////////////////////
static void DeleteNode(ProfilerNode* node)
{
	//delete node;
	node->~ProfilerNode();
	GetBlockAllocator()->ifree(node);
	//delete node;
}

////////////////////////////////
void ProfileInit()
{
	GetBlockAllocator()->Init(GetTrackedAllocator<char*>()
		, sizeof(ProfilerNode)
		, alignof(ProfilerNode)
		, 10
	);
}

////////////////////////////////
int GetTotalProfiledFrames()
{
	std::scoped_lock<std::shared_mutex> _(g_historyLock);
	return g_profileHistory.size();
}

////////////////////////////////
void ProfileReleaseTree(ProfilerNode* node)
{
	int newRefCount = ::InterlockedDecrement(const_cast<volatile long int*>(&(node->refCount)));
	if (newRefCount == 0) {
		ProfileFreeTree(node);
	}
}

////////////////////////////////
void ProfilePush(const char* tag)
{

	if ((!t_activeNode && t_pause) || (!t_activeNode && t_depth != 0)) {
		++t_depth;
		return;
	}
	++t_depth;
	ProfilerNode* newNode = NewNode();
	//t_activeNode->AddChild(newNode);
	newNode->beginHPC = GetCurrentHPC();
	strncpy_s(newNode->label, tag, std::min<size_t>(strlen(tag), PROFILER_LABEL_SIZE));
	if (t_activeNode) {
		t_activeNode->AddChild(newNode);
	}
	t_activeNode = newNode;
}

////////////////////////////////
void ProfilePop()
{
	--t_depth;
	if (!t_activeNode) {
		return;
	}
	t_activeNode->endHPC = GetCurrentHPC();

	if (t_activeNode->GetParent()) {
		t_activeNode = t_activeNode->GetParent();
	} else {
		std::scoped_lock<std::shared_mutex> _(g_historyLock);
		g_profileHistory.emplace_back(std::this_thread::get_id(), t_activeNode);
		if (g_profileHistory.size() > PROFILER_MAX_RECORD) {
			auto& release = g_profileHistory.front();
			ProfileReleaseTree(release.second);
			g_profileHistory.erase(g_profileHistory.begin());
		}
		t_activeNode = nullptr;
	}
}

////////////////////////////////
void ProfileFreeTree(ProfilerNode* root)
{
	if (root->m_firstChild) {
		ProfileFreeTree(root->m_firstChild);
		root->m_firstChild = nullptr;
	}
	if (root->GetNextSibling()) {
		ProfileFreeTree(root->GetNextSibling());
		root->SetNextSibling(nullptr);
	}
	DeleteNode(root);
}

////////////////////////////////
void ProfilePause()
{
	t_pause = true;
}

////////////////////////////////
void ProfileResume()
{
	t_pause = false;
}

////////////////////////////////
ProfilerNode* RequireReferenceOfProfileTree(std::thread::id threadID, int nFromBack/*=0*/)
{
	std::scoped_lock<std::shared_mutex> _(g_historyLock);
	int nCount = 0;
	for (int i = g_profileHistory.size() - 1; i >= 0; --i) {
		auto& hist = g_profileHistory[i];
		if (hist.first.threadID == threadID) {
			if (nCount == nFromBack) {
				::InterlockedIncrement(const_cast<volatile long int*>(&(hist.second->refCount)));
				return hist.second;
			}
			++nCount;
		}
	}
	return nullptr;
}

////////////////////////////////
std::vector<float> GetFrameTimeList()
{
	std::lock_guard<std::shared_mutex> lk(g_historyLock);
	std::vector<float> ret;
	for (auto& each : g_profileHistory) {
		ret.push_back((float)each.second->GetTimeMicroSecond());
	}
	return ret;
}

//////////////////////////////////////////////////////////////////////////
// PROFILE REPORT
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
#if MEM_TRACKING > MEM_TRACKING_DISABLE

#define PROFILE_REPORT_HEAD_FMT "%-36.36s %12.12s %12.12s %12.12s %12.12s %12.12s %9.9s %9.9s %18.18s"
#define PROFILE_REPORT_FMT      "%-36.36s %12d %9.3f¦Ìs %11.2f%% %9.3f¦Ìs %11.2f%% %9d %9d %18.18s"
#else
#define PROFILE_REPORT_HEAD_FMT "%-36.36s %12.12s %12.12s %12.12s %12.12s %12.12s"
#define PROFILE_REPORT_FMT      "%-36.36s %12d %10.3f¦Ìs %11.2f%% %10.3f¦Ìs %11.2f%%"
#endif

struct ProfilerReportNode
{
	std::string label;
	int calls = 0;
	double enclosedMicroSecond = 0;
	float enclosedPercent = 0;
	double childrenMicroSecond = 0;
	float childPercent = 0;

	int totalAllocs = 0;
	int totalFrees = 0;
	int deltaBytes = 0;

	ProfilerReportNode* firstChild = nullptr;
	ProfilerReportNode* nextSibling = nullptr;
	ProfilerReportNode* parent = nullptr;


	ProfilerReportNode* GetLastSibling() const
	{
		const ProfilerReportNode* p = this;
		while (p->nextSibling) {
			p = p->nextSibling;
		}
		return const_cast<ProfilerReportNode*>(p);
	}
};

////////////////////////////////
static bool _IsSelfTimeGreater(ProfilerReportNode*& a, ProfilerReportNode*& b)
{
	return
		a->enclosedMicroSecond - a->childrenMicroSecond
			>
		b->enclosedMicroSecond - b->childrenMicroSecond;
}

////////////////////////////////
static bool _IsTotalTimeGreater(ProfilerReportNode*& a, ProfilerReportNode*& b)
{
	return
		a->enclosedMicroSecond
			>
		b->enclosedMicroSecond;
}


////////////////////////////////
static void _Free_View(ProfilerReportNode* view);
static void _Gen_TreeView(ProfilerNode* profileTree, ProfilerReportNode* writeTo);
static void _ShowTreeView_Impl(ProfilerReportNode* report, int depth, bool sortBySelf = true);
static void _Gen_FlatView(ProfilerNode* profileTree, ProfilerReportNode* writeTo);
static void _ShowFlatView_Impl(ProfilerReportNode* report, int depth, bool sortBySelf = true);
////////////////////////////////
void ShowTreeView(ProfilerNode* profileTree, bool sortBySelf)
{
#if MEM_TRACKING > MEM_TRACKING_DISABLE
	Log("", PROFILE_REPORT_HEAD_FMT
		, "LABEL", "CAL", "ENC", "ENC%", "SLF", "SLF%", "ALC", "FRE", "DLT"
	);
#else
	Log("", PROFILE_REPORT_HEAD_FMT
		, "LABEL", "CAL", "ENC", "ENC%", "SLF", "SLF%"
	);
#endif
	ProfilerReportNode* report = new ProfilerReportNode();
	//report->calls = 1;
	report->label = profileTree->label;
	report->parent = nullptr;
	report->enclosedMicroSecond = profileTree->GetTimeMicroSecond();
	_Gen_TreeView(profileTree, report);
	_ShowTreeView_Impl(report, 0, sortBySelf);
	_Free_View(report);
}

////////////////////////////////
void ShowFlatView(ProfilerNode* profileTree, bool sortBySelf)
{
#if MEM_TRACKING > MEM_TRACKING_DISABLE
	Log("", PROFILE_REPORT_HEAD_FMT
		, "LABEL", "CAL", "ENC", "ENC%", "SLF", "SLF%", "ALC", "FRE", "DLT"
	);
#else
	Log("", PROFILE_REPORT_HEAD_FMT
		, "LABEL", "CAL", "ENC", "ENC%", "SLF", "SLF%"
	);
#endif
	ProfilerReportNode* report = new ProfilerReportNode();
	//report->calls = 1;
	report->label = "NULL";
	report->parent = nullptr;
	report->enclosedMicroSecond = profileTree->GetTimeMicroSecond();
	report->firstChild = new ProfilerReportNode();
	report->firstChild->parent = report;
	report->firstChild->label = profileTree->label;
	report->firstChild->enclosedMicroSecond = profileTree->GetTimeMicroSecond();
	_Gen_FlatView(profileTree, report);
	_ShowFlatView_Impl(report, 0, sortBySelf);
	_Free_View(report);
}

////////////////////////////////
void _Gen_TreeView(ProfilerNode* profileTree, ProfilerReportNode* writeTo)
{
	if (!profileTree || !writeTo) {
		return;
	}
	/*if (!writeTo) {
		writeTo = new ProfilerReportNode();
	}*/
	++writeTo->calls;
	writeTo->totalAllocs += profileTree->allocs;
	writeTo->totalFrees += profileTree->frees;
	writeTo->deltaBytes += profileTree->deltaByte;
	ProfilerNode* pChild = profileTree->m_firstChild;
	while (pChild) {
		ProfilerReportNode* pReport = writeTo->firstChild;
		while (pReport && pReport->label != pChild->label) {
			pReport = pReport->nextSibling;
		}
		if (!pReport) {
			//Gen new node
			ProfilerReportNode* newNode = new ProfilerReportNode();
			if (writeTo->firstChild) {
				writeTo->firstChild->GetLastSibling()->nextSibling = newNode;
			} else {
				writeTo->firstChild = newNode;
			}
			pReport = newNode;
			pReport->label = pChild->label;
			pReport->parent = writeTo;
		} else {
			writeTo->totalAllocs -= pReport->totalAllocs;
			writeTo->totalFrees -= pReport->totalFrees;
			writeTo->deltaBytes -= pReport->deltaBytes;
		}
		
		pReport->enclosedMicroSecond += pChild->GetTimeMicroSecond();
		writeTo->childrenMicroSecond += pChild->GetTimeMicroSecond();
		_Gen_TreeView(pChild, pReport);
		writeTo->totalAllocs += pReport->totalAllocs;
		writeTo->totalFrees += pReport->totalFrees;
		writeTo->deltaBytes += pReport->deltaBytes;

		pChild = pChild->m_nextSibling;
	}
}

////////////////////////////////
void _Free_View(ProfilerReportNode* view)
{
	if (!view) {
		return;
	}
	ProfilerReportNode* p = view->firstChild;
	while (p) {
		ProfilerReportNode* next = p->nextSibling;
		_Free_View(p);
		p = next;
	}
	delete view;
}

////////////////////////////////
void _ShowTreeView_Impl(ProfilerReportNode* report, int depth, bool sortBySelf)
{
	if (!report) {
		return;
	}

	std::string pre;
	for (int i = 0; i < depth; ++i) {
		pre.append(" ");
	}
	pre.append(report->label);

#if MEM_TRACKING > MEM_TRACKING_DISABLE
	Log(""
		, PROFILE_REPORT_FMT
		, pre.c_str()
		, report->calls
		, report->enclosedMicroSecond
		, report->parent ? report->enclosedMicroSecond / report->parent->enclosedMicroSecond * 100.0 : 100.00
		, report->enclosedMicroSecond - report->childrenMicroSecond
		, (report->enclosedMicroSecond - report->childrenMicroSecond) / report->enclosedMicroSecond * 100.0
		, report->totalAllocs
		, report->totalFrees
		, GetByteSizeString(report->deltaBytes).c_str()
	);
#else
	Log(""
		, PROFILE_REPORT_FMT
		, pre.c_str()
		, report->calls
		, report->enclosedMicroSecond
		, report->parent ? report->enclosedMicroSecond / report->parent->enclosedMicroSecond * 100.0 : 100.00
		, report->enclosedMicroSecond - report->childrenMicroSecond
		, (report->enclosedMicroSecond - report->childrenMicroSecond) / report->enclosedMicroSecond * 100.0
	);
#endif
	//Log("P", "%s%s\t%.5fus", pre.c_str(), report->label, report->GetTimeMicroSecond());

	std::vector<ProfilerReportNode*> children;
	ProfilerReportNode* child = report->firstChild;

	while (child) {
		children.push_back(child);
		child = child->nextSibling;
	}
	if (sortBySelf) {
		std::sort(std::begin(children), std::end(children), _IsSelfTimeGreater);
	} else {
		std::sort(std::begin(children), std::end(children), _IsTotalTimeGreater);
	}
	for (auto& each : children) {
		_ShowTreeView_Impl(each, depth + 1, sortBySelf);
	}
}

////////////////////////////////
void _Gen_FlatView(ProfilerNode* profileTree, ProfilerReportNode* writeTo)
{
	if (!profileTree || !writeTo) {
		return;
	}
	/*if (!writeTo) {
		writeTo = new ProfilerReportNode();
	}*/
	ProfilerNode* pChild = profileTree->m_firstChild;
	while (pChild) {
		ProfilerReportNode* pReport = writeTo->firstChild;
		while (pReport && pReport->label != pChild->label) {
			pReport = pReport->nextSibling;
		}
		if (!pReport) {
			//Gen new node
			ProfilerReportNode* newNode = new ProfilerReportNode();
			if (writeTo->firstChild) {
				writeTo->firstChild->GetLastSibling()->nextSibling = newNode;
			} else {
				writeTo->firstChild = newNode;
			}
			pReport = newNode;
			pReport->label = pChild->label;
			pReport->parent = writeTo;
		}
		pReport->totalAllocs += pChild->allocs;
		pReport->totalFrees += pChild->frees;
		pReport->deltaBytes += pChild->deltaByte;
		++pReport->calls;
		pReport->enclosedMicroSecond += pChild->GetTimeMicroSecond();
		writeTo->childrenMicroSecond += pChild->GetTimeMicroSecond();
		_Gen_FlatView(pChild, writeTo);

		pChild = pChild->m_nextSibling;
	}
}

////////////////////////////////
void _ShowFlatView_Impl(ProfilerReportNode* report, int depth, bool sortBySelf)
{
	if (!report) {
		return;
	}

	if (depth != 0) {
		std::string pre;
		for (int i = 0; i < depth; ++i) {
			pre.append(" ");
		}
		pre.append(report->label);
#if MEM_TRACKING > MEM_TRACKING_DISABLE
		Log(""
			, PROFILE_REPORT_FMT
			, pre.c_str()
			, report->calls
			, report->enclosedMicroSecond
			, report->parent ? report->enclosedMicroSecond / report->parent->enclosedMicroSecond * 100.0 : 100.00
			, report->enclosedMicroSecond - report->childrenMicroSecond
			, (report->enclosedMicroSecond - report->childrenMicroSecond) / report->parent->enclosedMicroSecond * 100.0
			, report->totalAllocs
			, report->totalFrees
			, GetByteSizeString(report->deltaBytes).c_str()
		);
#else
		Log(""
			, PROFILE_REPORT_FMT
			, pre.c_str()
			, report->calls
			, report->enclosedMicroSecond
			, report->parent ? report->enclosedMicroSecond / report->parent->enclosedMicroSecond * 100.0 : 100.00
			, report->enclosedMicroSecond - report->childrenMicroSecond
			, (report->enclosedMicroSecond - report->childrenMicroSecond) / report->enclosedMicroSecond * 100.0
		);
#endif
	}
	//Log("P", "%s%s\t%.5fus", pre.c_str(), report->label, report->GetTimeMicroSecond());

	std::vector<ProfilerReportNode*> children;
	ProfilerReportNode* child = report->firstChild;
	report->enclosedMicroSecond = 0;
	while (child) {
		children.push_back(child);
		report->enclosedMicroSecond += child->enclosedMicroSecond;
		child = child->nextSibling;
	}
	if (sortBySelf) {
		std::sort(std::begin(children), std::end(children), _IsSelfTimeGreater);
	} else {
		std::sort(std::begin(children), std::end(children), _IsTotalTimeGreater);
	}
	for (auto& each : children) {
		_ShowFlatView_Impl(each, depth + 1, sortBySelf);
	}
}

//////////////////////////////////////////////////////////////////////////
// This functions is for Memory TranckedAllocator to report allocation
//////////////////////////////////////////////////////////////////////////
void _NotifyAllocToProfiler(size_t bytes);
void _NotifyFreeToProfiler(size_t bytes);

////////////////////////////////
void _NotifyFreeToProfiler(size_t bytes)
{
	if (t_activeNode) {
		++t_activeNode->frees;
		t_activeNode->deltaByte -= bytes;
	}
}

////////////////////////////////
void _NotifyAllocToProfiler(size_t bytes)
{
	if (t_activeNode) {
		++t_activeNode->allocs;
		t_activeNode->deltaByte += bytes;
	}
}