#pragma once
#include "Worker.h"
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
class Job;

class JobManager
{
public:
	void Begin();
	void End();
	void AddJob(Job* pJob);
	void WaitForJobs();
	static std::atomic<int> mJobCounter;
	static bool GetEndFlag() { return EndFlag; }
	static Job* GetNextJob();
private:
	std::vector<Worker> mWorkers = std::vector<Worker>(4, Worker());
	static std::queue<Job*> mJobs;
	static std::mutex mMutex_Jobs;
	static bool EndFlag;
};

