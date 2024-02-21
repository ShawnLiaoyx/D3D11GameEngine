#include "stdafx.h"
#include "Worker.h"
#include "JobManager.h"
#include "Job.h"

void Worker::Begin()
{
	mThread = new std::thread(Loop);
}

void Worker::End()
{
	mThread->join();
	delete mThread;
}

void Worker::Loop()
{
	while (!JobManager::GetEndFlag()) {
		Job* pJob = JobManager::GetNextJob();
		if (pJob == nullptr) {
			std::this_thread::sleep_for(std::chrono::nanoseconds(1));
		}
		else {
			pJob->DoIt();
			--JobManager::mJobCounter;
			delete pJob;
		}
	}
}