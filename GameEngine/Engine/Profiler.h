#pragma once
#include <string>
#include <unordered_map>

#define PROFILE_SCOPE(name) \
Profiler::ScopedTimer name##_scope(Profiler::Get()->GetTimer(std::string(#name)))

class Profiler
{
public:
	class Timer
	{
	public:
		void Start();
		void Stop();
		void Reset();
		const std::string& GetName() const { return mTimerName; }
		double GetTime_ms() const { return mCurrFrameTime; }
		double GetMax_ms() const { return mMaxFrameTime; }
		double GetAvg_ms() const { return mTotalTime / mFrameNum; }
		friend class Profiler;
	private:
		Timer(const std::string& name);
		~Timer();
		std::string mTimerName;
		double mCurrFrameTime = 0.0;
		double mMaxFrameTime = 0.0;
		double mTotalTime = 0.0;
		int mFrameNum = 0;
		std::chrono::high_resolution_clock::time_point startTime;
	};

	class ScopedTimer 
	{
	public:
		ScopedTimer(Timer* t);
		~ScopedTimer();
	private:
		Timer* mTimer;
	};
	static Profiler* Get();
	Timer* GetTimer(const std::string& name);
	void ResetAll();
	void BeginTimer(const std::string& name, uint64_t startTime);
	void EndTimer(uint64_t endTime);
	friend class Timer;
private:
	Profiler();
	~Profiler();
	std::unordered_map<std::string, Timer*> mTimers;
	FILE* mTrace = NULL;
};

