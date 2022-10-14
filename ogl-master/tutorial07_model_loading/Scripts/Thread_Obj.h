#pragma once
#include <thread>
#include <stdio.h>
#include <functional>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Chunks_Manager;

class Thread_Obj
{
	friend class Thread_Manager;

public:
	Thread_Obj();
	~Thread_Obj();

	void TEST(Chunks_Manager* _chunkmanager, const glm::vec3& _loc);
	void MIAOU(const glm::vec3& _loc);

	void FinishThread();// DELETE ???

	inline void OnBehavior(const std::function<void(Thread_Obj*)>& _callback)
	{
		std::invoke(_callback, this);
	}

	/*template<typename ... Var>
	inline void AddThread(std::function<void(Var ...)> _callback, Var... _param)
	{
		std::invoke(_callback, this, _param);
	}

	inline void AddThread_NoParam(std::function<void(Thread_Obj*)> _callback)
	{
		std::invoke(_callback, this);
	}

	template<typename Param1>
	inline void AddThread_OneParam(std::function<void(Thread_Obj*)> _callback, Param1 _param)
	{
		std::invoke(_callback, this, _param);
		thread = std::thread(_callback, this, _param);
	}*/

private:

	inline void OnFinishThread(const std::function<void(Thread_Obj*)>& _callback)
	{
		onFinishThread = _callback;
	}

private:
			public://
	std::thread thread;
	Chunks_Manager* chunksManager;
	//Used to say to Thread_Manager thread is finished
	std::function<void(Thread_Obj*)> onFinishThread;
};