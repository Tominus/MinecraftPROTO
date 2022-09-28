#pragma once
#include "GlobalDefine.h"

#include <vector>

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

private:
	void Initialize();
	void SetMaxThread(const unsigned int& _quantity);

	//Maybe dangerous
	void InitializeThreads();

private:
	unsigned int maxSystemThread;
	unsigned int currentThreadQuantityUsed;

	std::vector<Thread_Obj*> threadObjs;
};