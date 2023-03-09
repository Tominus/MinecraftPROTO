#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <Windows.h>

#include "GlobalDefine.h"
#include "Action.h"

class Chunk;
class Thread;
class Thread_Manager;
class Chunk_Data_Generator;
class Chunk_Render_Generator;

class Chunks_Manager
{
	friend class World;

private:
	Chunks_Manager();
	~Chunks_Manager();

	/*DWORD WINAPI AddChunkTEST(const glm::vec3& _position)
	{
		Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _position);

		WaitForSingleObject(mutex, INFINITE);
		chunkWaitingForCGgen.push_back(_chunk);
		chunkPositionFinishGeneration.push_back(_position);
		ReleaseMutex(mutex);
		return 0;
	}

	void TestThradWin()
	{
		LPDWORD _currentThreadID = nullptr;
		HANDLE _currentThread = CreateThread(0, 0, (Thread_Func) runProcess, (void*)this, 0, _currentThreadID);

	}

	inline static int runProcess(void* pThis)
	{
		return ((Chunks_Manager*)(pThis))->AddChunkTEST(glm::vec3());
	}*/

private:
	static void WINAPI AddChunk(Thread* _thisThread, void* _pthis, const glm::vec3& _position);

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

	Action<> onUpdate;
	Action<> onTick;

	mutable HANDLE mutex;

	int renderDistance;
	int renderMaxDistance;
};