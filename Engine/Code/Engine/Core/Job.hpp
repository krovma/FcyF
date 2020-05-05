#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/AsyncQueue.hpp"
#include <functional>
#include <atomic>
#include <thread>

class Job;
class JobSystem;
class JobClass;

enum JobType
{
	JOB_GENERIC = 0,
	JOB_MAIN,
	JOB_RENDERING,
	JOB_IO,

	NUM_JOB_TYPES
};

class JobQueue
{
public:
	void Push(Job* job);
	Job* TryGetNextJob();
	Job* PollNextJob();

	void PushFinished(Job* job);
	Job* TryGetNextFinished();
	Job* PollNextFinished();
private:
	AsyncQueue<Job*> m_pending;
	AsyncQueue<Job*> m_finished;
};

void DO_NOTHING(Job*);

class Job
{
public:
	using JobFinishCallback = std::function<void(Job*)>;
	virtual void Run() = 0;
	void SetFinishCallback(JobFinishCallback callback);
	void SetCatagory(JobType type) { m_jobtype = type; }
private:
	void _AddSuccessor(Job* successor);
	void _AddPredecessor(Job* predecessor);
	bool _TryRun();
	void _Finish();
private:
	JobType m_jobtype = JOB_GENERIC;
	JobFinishCallback m_callback = DO_NOTHING;
	std::vector<Job*> m_successors;
	std::atomic<int> m_predecessorCount = 1;

	friend class JobSystem;
	friend class JobQueue;
};

class JobSystem
{
public:
	// negative means as many as you can MINUS the value (8 cores, -1 is 7... -2 would be 6.  -20 would be 1)
	// MINIMUM 1 unless explicitly saying 0; 
	void Startup(int nGenericThreads = 1, int numCatagories = NUM_JOB_TYPES);
	void Shutdown();
	void Run(Job* job);
	void AddDependency(Job* first, Job* second);
	bool IsRunning() const { return m_isRunning; }
	bool IsFinished() const;
	int ProcessQueueForMS(JobType jobType, unsigned int ms);
	int ProcessQueue(JobType jobType);
	void FinishJobsQueue(JobType jobType);
private:
	bool m_isRunning = true;
};

extern JobSystem* g_theJobSystem;