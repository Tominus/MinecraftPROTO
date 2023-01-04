#pragma once
#include <vector>
#include <mutex>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdio.h>//
#include <functional>//

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

	mutable std::mutex mutex;

	int renderDistance;
	int renderMaxDistance;
};