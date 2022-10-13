#pragma once
#include <vector>

#include <GL/glew.h>
#include <glm/glm.hpp>

class Chunk;
class Chunk_Data_Generator;
class Chunk_Render_Generator;

#include <stdio.h>//
#include <functional>//
class Chunks_Manager
{
	friend class World;

private:
	Chunks_Manager();
	~Chunks_Manager();
public://
	void AddChunk(const glm::vec3& _position);
	void UpdateChunksManager() const;

	std::function<void()> onUpdate;//
	void CheckForNewChunk();//
	void UpdateRender();

public:
	Chunk* GetChunkAtPosition(const glm::vec3& _position) const;

	inline Chunk_Data_Generator* GetChunkDataGenerator() const { return chunkDataGenerator; }
	inline Chunk_Render_Generator* GetChunkRenderGenerator() const { return chunkRenderGenerator; }

private:
	std::vector<Chunk*> worldChunks;

	Chunk_Data_Generator* chunkDataGenerator;
	Chunk_Render_Generator* chunkRenderGenerator;

	//
	std::vector<Chunk*> chunkWaitingForCGgen;
	//
};