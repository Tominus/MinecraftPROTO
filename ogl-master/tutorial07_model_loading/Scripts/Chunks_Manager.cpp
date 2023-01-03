#include "Chunks_Manager.h"

#include "World.h"
#include "Chunk.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"
#include <common/controls.hpp>
#include "Thread_Manager.h"
#include "Thread_Obj.h"

Chunks_Manager::Chunks_Manager()
{
	chunkDataGenerator = new Chunk_Data_Generator(this);
	chunkRenderGenerator = new Chunk_Render_Generator(this);

	renderDistance = Render_Distance_Current;
	renderMaxDistance = renderDistance - 1;

	onUpdate = [this]()
	{
		UpdateRender();
	};

	onTick = [this]()
	{
		CheckGenerateNewChunkRender();
		CheckRenderDistance();
	};


	/*Thread_Manager* _threadManager = &Thread_Manager::Instance();
	{
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3());
		//d
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, -1, 0));
		//u
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, 1, 0));
		//l
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, 0, 0));
		//r
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, 0, 0));
		//b
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, 0, -1));
		//f
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, 0, 1));
	}

	{
		//l
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, 0, 1));
		//r
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, 0, -1));
		//b
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, 0, -1));
		//f
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, 0, 1));
	}

	{
		//l
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, -1, 1));
		//r
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, -1, -1));
		//b
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, -1, -1));
		//f
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, -1, 1));
	}

	{
		//l
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, 1, 1));
		//r
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, 1, -1));
		//b
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, 1, -1));
		//f
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, 1, 1));
	}

	{
		//ld
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, -1, 0));
		//rd
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, -1, 0));
		//bd
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, -1, -1));
		//fd
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, -1, 1));
	}

	{
		//lu
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(-1, 1, 0));
		//ru
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(1, 1, 0));
		//bu
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, 1, -1));
		//fu
		if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
			_thread->TEST(this, glm::vec3(0, 1, 1));
	}*/
}

Chunks_Manager::~Chunks_Manager()
{
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		delete worldChunks[i];
	}

	const size_t& _max2 = chunkWaitingForCGgen.size();
	for (size_t i = 0; i < _max2; ++i)
	{
		delete chunkWaitingForCGgen[i];
	}

	delete chunkDataGenerator;
	delete chunkRenderGenerator;
}

void Chunks_Manager::AddChunk(const glm::vec3& _position)
{
	Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _position);
	mutex.lock();
	chunkWaitingForCGgen.push_back(_chunk);
	mutex.unlock();
}

Chunk* Chunks_Manager::GetChunkAtPosition(const glm::vec3& _position) const
{
	mutex2.lock();
	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk* _chunk = worldChunks[i];
		if (_position == _chunk->chunkPosition)
		{
			mutex2.unlock();
			return _chunk;
		}
	}
	mutex2.unlock();
	return nullptr;
}

void Chunks_Manager::UpdateChunksManager() const
{
	std::invoke(onUpdate);
}

void Chunks_Manager::TickChunksManager() const
{
	std::invoke(onTick);
}

void Chunks_Manager::CheckGenerateNewChunkRender()
{
	mutex.lock();
	size_t _size (chunkWaitingForCGgen.size());
	while (_size > 0)
	{
		Chunk* _chunk = chunkWaitingForCGgen[0];
		_chunk->GenerateCGRender();

		chunkWaitingForCGgen.erase(chunkWaitingForCGgen.begin());
		--_size;

		worldChunks.push_back(_chunk);
	}
	mutex.unlock();
}

void Chunks_Manager::UpdateRender()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	
	const size_t& _max = worldChunks.size();

	for (size_t i(0); i < _max; ++i)
		worldChunks[i]->Render();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Chunks_Manager::CheckRenderDistance()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8, 8, 8);
	const glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	std::vector<Chunk*> _inRenderDistanceChunks;

	glm::vec3 _offset;
	for (int x = -renderMaxDistance; x < renderDistance; ++x)
	{
		_offset.x = x;
		for (int z = -renderMaxDistance; z < renderDistance; ++z)
		{
			_offset.z = z;

			const glm::vec3& _offsettedPlayerRelativePosition = _offset + _playerPositionChunkRelative;
			if (Chunk* _chunk = GetChunkAtPosition(_offsettedPlayerRelativePosition))
			{
				_inRenderDistanceChunks.push_back(_chunk);
			}
			else
			{
				Thread_Manager* _threadManager = &Thread_Manager::Instance();
				if (Thread_Obj* _thread = _threadManager->GetValidThreadObj())
					_thread->TEST(this, _offsettedPlayerRelativePosition);
			}
		}
	}

	size_t _max = worldChunks.size();
	const size_t& _max2 = _inRenderDistanceChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk*& _worldChunk = worldChunks[i];

		bool _isInRange = false;
		for (size_t j = 0; j < _max2; ++j)
		{
			if (_worldChunk == _inRenderDistanceChunks[j])
			{
				_isInRange = true;
				break;
			}
		}

		if (!_isInRange)
		{
			delete _worldChunk;
			_worldChunk = nullptr;
		}
	}
	
	for (size_t i = 0; i < _max; ++i)
	{
		if (!worldChunks[i])
		{
			worldChunks.erase(worldChunks.begin() + i);
			i--;
			_max--;
		}
	}
}