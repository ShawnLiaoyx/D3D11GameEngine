#include "stdafx.h"
#include "JobManager.h"
#include "Job.h"

bool JobManager::EndFlag = false;
std::queue<Job*> JobManager::mJobs;
std::mutex JobManager::mMutex_Jobs;
std::atomic<int> JobManager::mJobCounter = 0;

void JobManager::Begin()
{
	for (auto& w : mWorkers) {
		w.Begin();
	}
}

void JobManager::End()
{
	JobManager::EndFlag = true;
	for (auto& w : mWorkers) {
		w.End();
	}
}

void JobManager::AddJob(Job* pJob)
{
	//updated the job counter
	++mJobCounter;

	//request the lock and push pJob
	std::lock_guard<std::mutex> lock(mMutex_Jobs);
	mJobs.push(pJob);
	//the lock is released automatically
}

void JobManager::WaitForJobs()
{
	//loop until job counter gets to 0
	while (JobManager::mJobCounter > 0) {
		std::this_thread::sleep_for(std::chrono::nanoseconds(1));
	}
}

Job* JobManager::GetNextJob()
{
	//Thread safe getter for job
	//Request a log first
	std::lock_guard<std::mutex> lock(mMutex_Jobs);

	//If empty, returns nullptr
	if (mJobs.empty()) {
		return nullptr;
	}

	//otherwise return the front element and pop()
	Job* result = mJobs.front();
	mJobs.pop();
	return result;
	//The lock is released automatically
}