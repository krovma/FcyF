#include "Engine/Core/Job.hpp"
#include <algorithm>
#include "Engine/Core/Time.hpp"

//////////////////////////////////////////////////////////////////////////
static std::vector<JobQueue*> _JobQueues;
static std::vector<std::thread> _GeneraicThread;

////////////////////////////////
void DO_NOTHING(Job*)
{
	//
}

JobSystem* g_theJobSystem = nullptr;
//static std::vector<std::thread> _TypedThread;
//////////////////////////////////////////////////////////////////////////
////////    CLASS JOB                   //////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void Job::SetFinishCallback(JobFinishCallback callback)
{
	m_callback = callback;
}

////////////////////////////////
void Job::_AddSuccessor(Job* successor)
{
	for (const auto& each : m_successors) {
		if (each == successor) {
			return;
		}
	}
	m_successors.push_back(successor);
	++successor->m_predecessorCount;
}

////////////////////////////////
void Job::_AddPredecessor(Job* predecessor)
{
	predecessor->_AddSuccessor(this);
}

////////////////////////////////
bool Job::_TryRun()
{
	int dec = --m_predecessorCount;
	if (dec > 0) {
		return false;
	}
	_JobQueues[m_jobtype]->Push(this);
	return true;
}

////////////////////////////////
void Job::_Finish()
{
	for (auto& each : m_successors) {
		each->_TryRun();
	}
	if (m_callback) {
		std::invoke(m_callback, this);
	}
}

//////////////////////////////////////////////////////////////////////////
//////CLASS JOBSYSTEM                       //////////////////////////////
//////////////////////////////////////////////////////////////////////////

static void GenericJobThread()
{
	thread_local JobQueue* const genericJobQueue = _JobQueues[JOB_GENERIC];
	while (g_theJobSystem->IsRunning()) {
		Job* job = genericJobQueue->PollNextJob();
		if (job) {
			job->Run();
			genericJobQueue->PushFinished(job);
		}
	}
}

////////////////////////////////
void JobSystem::Startup(int nGenericThreads /*=1*/, int numCatagories /*= NUM_JOB_TYPES*/)
{
	for (int i = 0; i < NUM_JOB_TYPES; ++i) {
		_JobQueues.push_back(new JobQueue());
	}
	if (nGenericThreads < 0) {
		nGenericThreads = std::max(1, (int)std::thread::hardware_concurrency() - (-nGenericThreads));
	}
	for (int i = 0; i < nGenericThreads; ++i) {
		_GeneraicThread.emplace_back(GenericJobThread);
		std::thread& th = _GeneraicThread[i];
		th.detach();
	}
}

////////////////////////////////
void JobSystem::Shutdown()
{
	m_isRunning = false;
}

////////////////////////////////
void JobSystem::Run(Job* job)
{
	//JobQueue* jq = _JobQueues[job->m_jobtype];
	//jq->Push(job);
	job->_TryRun();
}

////////////////////////////////
void JobSystem::AddDependency(Job* first, Job* second)
{
	first->_AddSuccessor(second);
}

////////////////////////////////
bool JobSystem::IsFinished() const
{
	return true;
}

////////////////////////////////
int JobSystem::ProcessQueueForMS(JobType jobType, unsigned int ms)
{
	JobQueue* jobQueue = _JobQueues[jobType];
	double current = HPCToSeconds(GetCurrentHPC()) * 1000;
	int count = 0;
	while (HPCToSeconds(GetCurrentHPC()) * 1000 - current < (double)ms) {
		Job* job = jobQueue->TryGetNextJob();
		if (!job) {
			break;
		}
		++count;
		job->Run();
		jobQueue->PushFinished(job);
	}
	return count;
}

////////////////////////////////
int JobSystem::ProcessQueue(JobType jobType)
{
	JobQueue* jobQueue = _JobQueues[jobType];
	int count = 0;
	while (true) {
		Job* job = jobQueue->TryGetNextJob();
		if (!job) {
			break;
		}
		++count;
		job->Run();
		jobQueue->PushFinished(job);
	}
	return count;
}

////////////////////////////////
void JobSystem::FinishJobsQueue(JobType jobType)
{
	JobQueue* jobQueue = _JobQueues[jobType];
	while (true) {
		Job* job = jobQueue->TryGetNextFinished();
		if (!job) {
			break;
		}
		job->_Finish();
		delete job;
		//jobQueue->PushFinished(job);
	}
}

//////////////////////////////////////////////////////////////////////////
/////CLASS JOB QUEUE         /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void JobQueue::Push(Job* job)
{
	m_pending.Push(job);
}

////////////////////////////////
Job* JobQueue::TryGetNextJob()
{
	Job* job = nullptr;
	if (m_pending.Pop(&job)) {
		return job;
	}
	return nullptr;
}

////////////////////////////////
Job* JobQueue::PollNextJob()
{
	Job* job = nullptr;
	while (!m_pending.Pop(&job)) {
		if (!g_theJobSystem->IsRunning()) {
			return nullptr;
		}
		std::this_thread::yield();
	}
	return job;
}

////////////////////////////////
void JobQueue::PushFinished(Job* job)
{
	m_finished.Push(job);
}

////////////////////////////////
Job* JobQueue::TryGetNextFinished()
{
	Job* job = nullptr;
	if (m_finished.Pop(&job)) {
		return job;
	}
	return nullptr;
}

////////////////////////////////
Job* JobQueue::PollNextFinished()
{
	Job* job = nullptr;
	while (!m_finished.Pop(&job)) {
		if (!g_theJobSystem->IsRunning()) {
			return nullptr;
		}
		std::this_thread::yield();
	}
	return job;
}

