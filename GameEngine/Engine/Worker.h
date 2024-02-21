#pragma once
#include <thread>
class Worker
{
public:
	void Begin();
	void End();
	static void Loop();
	
private:
	std::thread* mThread = nullptr;
};

