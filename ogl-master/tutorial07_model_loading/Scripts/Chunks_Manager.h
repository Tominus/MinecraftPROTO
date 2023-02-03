#pragma once
#include <vector>
#include <mutex>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdio.h>//
#include <functional>//
#include <Windows.h>
#include <stdio.h>

#include "Chunk.h"//TODO delete

class Chunk;
class Thread_Manager;
class Chunk_Data_Generator;
class Chunk_Render_Generator;

class Chunks_Manager
{
	friend class World;

private:
	Chunks_Manager();
	~Chunks_Manager();
public://
	void AddChunk(const glm::vec3& _position);



	DWORD WINAPI AddChunkTEST(void* _params)
	{
		const glm::vec3 _position = (glm::vec3&&)_params;
		Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, (glm::vec3&&)_params);
		//mutex.lock();
		mutex = CreateMutexA(0, false, 0);
		//_chunk->GenerateCGRender();
		worldChunks.push_back(_chunk);
		chunkWaitingForCGgen.push_back(_chunk);
		chunkPositionFinishGeneration.push_back((glm::vec3&&)_params);
		//mutex.unlock();
		ReleaseMutex(mutex);
		return 0;
	}

	void TestThradWin()
	{
		LPDWORD _currentThreadID = nullptr;
		HANDLE _currentThread = CreateThread(0, 0, (unsigned long(__stdcall*)(void*))runProcess, (void*)this, 0, _currentThreadID);

	}

	static int runProcess(void* pThis)
	{
		glm::vec3* _test = new glm::vec3(0.f, 0.f, 0.f);

		return ((Chunks_Manager*)(pThis))->AddChunkTEST(_test);
	}


	//LPTHREAD_START_ROUTINE

private:
	void AddStartingWorldBaseChunk();

	void UpdateChunksManager() const;
	void TickChunksManager() const;

	void UpdateRender();

	void CheckGenerateNewChunkRender();
	void CheckGenerateChunkPosition();

	void CheckRenderDistance();

public:
	Chunk* GetChunkAtPosition(const glm::vec3& _position) const;
	bool GetIsChunkAtPositionBeingGenerated(const glm::vec3& _position) const;

	inline Chunk_Data_Generator* GetChunkDataGenerator() const { return chunkDataGenerator; }
	inline Chunk_Render_Generator* GetChunkRenderGenerator() const { return chunkRenderGenerator; }

private:
	std::vector<Chunk*> worldChunks;

	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;
	Thread_Manager* threadManager;

	std::vector<Chunk*> chunkWaitingForCGgen;
	std::vector<glm::vec3> chunkPositionBeingGenerated;
	std::vector<glm::vec3> chunkPositionFinishGeneration;

	std::function<void()> onUpdate;
	std::function<void()> onTick;

	//mutable std::mutex mutex;
	mutable HANDLE mutex;

	int renderDistance;
	int renderMaxDistance;
};