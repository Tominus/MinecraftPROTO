#pragma once
#include "GlobalDefine.h"

#include <vector>
#include <Windows.h>

#include "Thread_Obj.h"

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
	inline static Thread_Manager& Instance()
	{
		static Thread_Manager instance;
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