#pragma once
#include "GlobalDefine.h"

#include <vector>
#include <mutex>

class Thread_Obj;

/**
* The Manager of thread.
* 
* Can be called by everyone to used thread 
*/
class Thread_Manager
{

	friend class MainGame;

private:
	Thread_Manager() {};
	~Thread_Manager();

public:
	inline static Thread_Manager& Instance()
	{
		static Thread_Manager instance;
		return instance;
	}

	void SetMaxThread(const unsigned int& _quantity);

	inline bool GetHasAllThreadFinished() const { return invalidThreadObjs.size() == 0; }

private:
	void Initialize();
	void InitializeThreads();

	void Internal_SetMaxThread(unsigned int _quantity);
	void ResetThreadObjs();//TODO
	void InterruptThreadObjs();

	void DetachInvalidThread(Thread_Obj* _thread);

public:
	void SetThreadBehaviorFinished(Thread_Obj* _thread);

	Thread_Obj* GetValidThreadObj();

private:
	unsigned int maxSystemThread;
	unsigned int currentThreadQuantityUsed;

	std::vector<Thread_Obj*> threadObjs;

	std::vector<Thread_Obj*> validThreadObjs;
	std::vector<Thread_Obj*> invalidThreadObjs;

	std::mutex mutex_InvalidThreadObjs;
};