#pragma once
#include <thread>
#include <stdio.h>
#include <functional>

class Thread_Obj
{
	friend class Thread_Manager;

public:
	Thread_Obj();
	~Thread_Obj();


	void FinishThread();
	

private:

	inline void OnFinishThread(std::function<void(Thread_Obj*)> _callback)
	{
		callback = _callback;
	}

private:
	std::thread* thread;

	std::function<void(Thread_Obj*)> callback;
};