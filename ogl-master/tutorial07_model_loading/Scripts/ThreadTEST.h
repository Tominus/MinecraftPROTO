#pragma once
#include <stdio.h>
#include <iostream>
#include <thread>

class ThreadTEST
{
public:
	ThreadTEST();
	~ThreadTEST();

	void StartThreadingTEST();

	void Thread_1()
	{
		scanf("UwU 1");
	}
	void Thread_2(int x)
	{
		scanf("UwU %d", x);
	}

private:
};