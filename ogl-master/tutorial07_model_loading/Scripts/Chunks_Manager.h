#pragma once
#include <vector>
#include <mutex>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdio.h>//
#include <functional>//

class Chunk;
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
	void UpdateChunksManager() const;
	void TickChunksManager() const;

	void CheckGenerateNewChunkRender();
	void UpdateRender();

	void CheckRenderDistance();

public:
	Chunk* GetChunkAtPosition(const glm::vec3& _position) const;

	inline Chunk_Data_Generator* GetChunkDataGenerator() const { return chunkDataGenerator; }
	inline Chunk_Render_Generator* GetChunkRenderGenerator() const { return chunkRenderGenerator; }

private:
	std::vector<Chunk*> worldChunks;

	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;

	std::vector<Chunk*> chunkWaitingForCGgen;

	std::function<void()> onUpdate;
	std::function<void()> onTick;

	std::mutex mutex_chunkWaitingForCGgen;

	int renderDistance;
	int renderMaxDistance;
};