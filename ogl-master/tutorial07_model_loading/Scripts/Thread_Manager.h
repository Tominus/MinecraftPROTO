#pragma once
#include "GlobalDefine.h"

#include "Thread.h"

#include <vector>
#include <Windows.h>

/**
* The Manager of thread.
* 
* Can be called by everyone to used thread 
*/
class Thread_Manager
{

	friend class MainGame;

private:
	Thread_Manager();
	~Thread_Manager();

public:
	inline static Thread_Manager* Instance()
	{
		static Thread_Manager* instance = new Thread_Manager();
		return instance;
	}

	bool GetHasAllThreadFinished() const { return allCurrentThread.size() == 0; }
	
	Thread* CreateThread();

private:
	void DeleteFinishedThread(Thread* _thread);

private:
	HANDLE mutex;

	HANDLE mainThread;
	DWORD_PTR mainThreadAffinity;

	std::vector<Thread*> allCurrentThread;
};