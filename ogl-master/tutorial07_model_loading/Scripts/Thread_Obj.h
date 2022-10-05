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


	void FinishThread();// DELETE ???

	inline void OnBehavior(const std::function<void(Thread_Obj*)>& _callback)
	{
		std::invoke(_callback, this);
	}

	inline void OnTest(const std::function<void(Thread_Obj*)>& _callback)
	{
		std::invoke(_callback, this);
	}

private:

	inline void OnFinishThread(const std::function<void(Thread_Obj*)>& _callback)
	{
		onFinishThread = _callback;
	}

private:
			public://
	std::thread thread;

	//Used to say to Thread_Manager thread is finished
	std::function<void(Thread_Obj*)> onFinishThread;
};