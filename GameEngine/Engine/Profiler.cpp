#include "stdafx.h"
#include "Profiler.h"
#include <stdio.h>
#include <thread>

Profiler::Profiler()
{
	errno_t err;
	err = fopen_s(&mTrace, "profile.json", "w");
	if (!err && mTrace != NULL) {
		fputc('[', mTrace);
		fputc('\n', mTrace);
	}
}

Profiler::~Profiler()
{
	FILE* fp;
	errno_t err;
	err = fopen_s(&fp, "profile.txt", "w");
	if (!err && fp != NULL) {
		fprintf(fp, "name:, avg (ms), max (ms)\n");
		for (auto& p : mTimers) {
			fprintf(fp, "%s:, %f, %f\n", p.second->GetName().c_str(), 
				p.second->GetAvg_ms(), p.second->GetMax_ms());
		}
		fclose(fp);
	}
	if (mTrace != NULL) {
		fseek(mTrace, -3, SEEK_CUR);
		fputc('\n', mTrace);
		fputc(']', mTrace);
		fclose(mTrace);
	}
	for (auto& p : mTimers) {
		delete p.second;
	}
}

Profiler::Timer::Timer(const std::string& name)
	:mTimerName(name)
{
}

Profiler::Timer::~Timer()
{
}

void Profiler::Timer::Start() 
{
	startTime = _CHRONO high_resolution_clock::now(); 
	Profiler::Get()->BeginTimer(mTimerName, startTime.time_since_epoch().count() / 1000);
}

void Profiler::Timer::Stop() 
{
	const _CHRONO high_resolution_clock::time_point& endTime = _CHRONO high_resolution_clock::now();
	mCurrFrameTime += _CHRONO duration<double, std::milli>(endTime - startTime).count();
	Profiler::Get()->EndTimer(endTime.time_since_epoch().count() / 1000);
}

void Profiler::Timer::Reset() 
{
	mTotalTime += mCurrFrameTime;
	mMaxFrameTime = max(mMaxFrameTime, mCurrFrameTime);
	++mFrameNum;
	mCurrFrameTime = 0.0;
}

Profiler* Profiler::Get() 
{
	static Profiler profiler;
	return &profiler;
}

Profiler::Timer* Profiler::GetTimer(const std::string& name)
{
	auto itr = mTimers.find(name);
	if (itr != mTimers.end()) {
		return itr->second;
	}
	else
	{
		Timer* t = new Timer(name);
		mTimers.emplace(name, t);
		return t;
	}
}

void Profiler::ResetAll()
{
	for (auto& p : mTimers) {
		p.second->Reset();
	}
}

Profiler::ScopedTimer::ScopedTimer(Timer* t) 
	:mTimer(t)
{
	mTimer->Start();
}

Profiler::ScopedTimer::~ScopedTimer()
{
	mTimer->Stop();
}

void Profiler::BeginTimer(const std::string& name, uint64_t startTime)
{
	fprintf(mTrace, 
		"{\"name\": \"%s\", \"ph\": \"B\", \"ts\": %llu, \"pid\": 1, \"tid\": %llu},\n", 
		name.c_str(), startTime, std::hash<std::thread::id>{}(std::this_thread::get_id()));
}

void Profiler::EndTimer(uint64_t endTime)
{
	fprintf(mTrace, "{\"ph\": \"E\", \"ts\": %llu, \"pid\": 1, \"tid\": %llu},\n",
		endTime, std::hash<std::thread::id>{}(std::this_thread::get_id()));
}