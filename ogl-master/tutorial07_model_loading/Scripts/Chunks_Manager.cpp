#include "Chunks_Manager.h"

#include "World.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"
#include <common/controls.hpp>
#include "Thread_Manager.h"
#include "Thread_Obj.h"

#include <Windows.h>
#include <stdio.h>


Chunks_Manager::Chunks_Manager()
{
	chunkDataGenerator = new Chunk_Data_Generator(this);
	chunkRenderGenerator = new Chunk_Render_Generator(this);
	threadManager = &Thread_Manager::Instance();

	renderDistance = Render_Distance_Current;
	renderMaxDistance = renderDistance - 1;

	mutex = CreateMutex(0, false, 0);

	onUpdate.AddDynamic(this, &Chunks_Manager::UpdateRender);

	onTick.AddDynamic(this, &Chunks_Manager::CheckGenerateNewChunkRender);
	onTick.AddDynamic(this, &Chunks_Manager::CheckGenerateChunkPosition);
	onTick.AddDynamic(this, &Chunks_Manager::CheckRenderDistance);

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

	/*AddChunk(glm::vec3(0, -1, 0));
	AddChunk(glm::vec3(0, 1, 0));
	AddChunk(glm::vec3(-1, 0, 0));
	AddChunk(glm::vec3(1, 0, 0));
	AddChunk(glm::vec3(0, 0, -1));
	AddChunk(glm::vec3(0, 0, 1));

	AddChunk(glm::vec3(1, 0, 1));
	AddChunk(glm::vec3(-1, 0, -1));
	AddChunk(glm::vec3(1, 0, -1));
	AddChunk(glm::vec3(-1, 0, 1));

	AddChunk(glm::vec3(1, -1, 1));
	AddChunk(glm::vec3(-1, -1, -1));
	AddChunk(glm::vec3(1, -1, -1));
	AddChunk(glm::vec3(-1, -1, 1));

	AddChunk(glm::vec3(1, 1, 1));
	AddChunk(glm::vec3(-1, 1, -1));
	AddChunk(glm::vec3(1, 1, -1));
	AddChunk(glm::vec3(-1, 1, 1));

	AddChunk(glm::vec3(1, -1, 0));
	AddChunk(glm::vec3(-1, -1, 0));
	AddChunk(glm::vec3(0, -1, -1));
	AddChunk(glm::vec3(0, -1, 1));

	AddChunk(glm::vec3(-1, 1, 0));
	AddChunk(glm::vec3(1, 1, 0));
	AddChunk(glm::vec3(0, 1, -1));
	AddChunk(glm::vec3(0, 1, 1));*/
}

Chunks_Manager::~Chunks_Manager()
{
	WaitForSingleObject(mutex, INFINITE);

	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk*& _chunk = worldChunks[i];
		delete _chunk;
		_chunk = nullptr;
	}
	ReleaseMutex(mutex);

	CloseHandle(mutex);

	onUpdate.RemoveDynamic(this, &Chunks_Manager::UpdateRender);

	onTick.RemoveDynamic(this, &Chunks_Manager::CheckGenerateNewChunkRender);
	onTick.RemoveDynamic(this, &Chunks_Manager::CheckGenerateChunkPosition);
	onTick.RemoveDynamic(this, &Chunks_Manager::CheckRenderDistance);

	const size_t& _max2 = chunkWaitingForCGgen.size();
	for (size_t i = 0; i < _max2; ++i)
	{
		delete chunkWaitingForCGgen[i];
	}

	delete chunkDataGenerator;
	delete chunkRenderGenerator;
}

void Chunks_Manager::AddChunk(SThread_AddChunk_Ptr _data)
{	
	const glm::vec3& _position = *_data->position;
	Chunks_Manager* _pthis = _data->thisPtr;
	Thread* _thisThread = _data->thisThread;


	Chunk* _chunk = new Chunk(_pthis->chunkDataGenerator, _pthis->chunkRenderGenerator, _position);

	WaitForSingleObject(_pthis->mutex, INFINITE);
	_pthis->chunkWaitingForCGgen.push_back(_chunk);
	_pthis->chunkPositionFinishGeneration.push_back(_position);
	ReleaseMutex(_pthis->mutex);

	_thisThread->OnFinished.Invoke(_thisThread);

	delete _data->position;
	delete _data;
}

void Chunks_Manager::AddStartingWorldBaseChunk()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8.f);
	const glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);

	Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _playerPositionChunkRelative);

	WaitForSingleObject(mutex, INFINITE);
	chunkWaitingForCGgen.push_back(_chunk);
	chunkPositionFinishGeneration.push_back(_playerPositionChunkRelative);
	ReleaseMutex(mutex);
}

void Chunks_Manager::UpdateChunksManager() const
{
	onUpdate.Invoke();
}

void Chunks_Manager::TickChunksManager() const
{
	onTick.Invoke();
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

void Chunks_Manager::CheckGenerateNewChunkRender()
{
	WaitForSingleObject(mutex, INFINITE);

	size_t _size (chunkWaitingForCGgen.size());
	while (_size > 0)
	{
		Chunk* _chunk = chunkWaitingForCGgen[0];
		_chunk->GenerateCGRender();

		chunkWaitingForCGgen.erase(chunkWaitingForCGgen.begin());
		--_size;

		worldChunks.push_back(_chunk);
	}
	ReleaseMutex(mutex);
}

void Chunks_Manager::CheckGenerateChunkPosition()
{
	WaitForSingleObject(mutex, INFINITE);

	size_t _size(chunkPositionFinishGeneration.size());
	size_t _index (0);

	while (_size > 0)
	{
		const glm::vec3& _finishPosition = chunkPositionFinishGeneration[0];

		const size_t& _max = chunkPositionBeingGenerated.size();
		for (_index = 0; _index < _max; ++_index)
		{
			if (chunkPositionBeingGenerated[_index] == _finishPosition)
				break;
		}

		chunkPositionBeingGenerated.erase(chunkPositionBeingGenerated.begin() + _index);
		chunkPositionFinishGeneration.erase(chunkPositionFinishGeneration.begin());
		--_size;
	}
	ReleaseMutex(mutex);
}

void Chunks_Manager::CheckRenderDistance()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8, 8, 8);
	const glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	size_t _worldChunkSize = worldChunks.size();
	std::vector<Chunk*> _inRenderDistanceChunks;

	if (_worldChunkSize == 0)
	{
		if (chunkPositionBeingGenerated.size() == 0 || chunkPositionFinishGeneration.size() == 0)
		{
			if (Thread* _thread = threadManager->CreateThread())
			{
				SThread_AddChunk_Ptr _data = new SThread_AddChunk();
				_data->thisThread = _thread;
				_data->thisPtr = this;
				_data->position = new glm::vec3(_playerPositionChunkRelative);
				_thread->CreateThreadFunction(true, AddChunk, _data);
				chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);
			}
		}
		return;
	}

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
				if (!GetIsChunkAtPositionBeingGenerated(_offsettedPlayerRelativePosition))
				{
					if (GetChunkAtPosition(_offsettedPlayerRelativePosition + glm::vec3(0, -1, 0))
						|| GetChunkAtPosition(_offsettedPlayerRelativePosition + glm::vec3(0, 1, 0))
						|| GetChunkAtPosition(_offsettedPlayerRelativePosition + glm::vec3(-1, 0, 0))
						|| GetChunkAtPosition(_offsettedPlayerRelativePosition + glm::vec3(1, 0, 0))
						|| GetChunkAtPosition(_offsettedPlayerRelativePosition + glm::vec3(0, 0, -1))
						|| GetChunkAtPosition(_offsettedPlayerRelativePosition + glm::vec3(0, 0, 1)))
					{
						if (Thread* _thread = threadManager->CreateThread())
						{
							SThread_AddChunk_Ptr _data = new SThread_AddChunk();
							_data->thisThread = _thread;
							_data->thisPtr = this;
							_data->position = new glm::vec3(_offsettedPlayerRelativePosition);
							_thread->CreateThreadFunction(true, AddChunk, _data);
							chunkPositionBeingGenerated.push_back(_offsettedPlayerRelativePosition);
						}
					}
				}
			}
		}
	}

	WaitForSingleObject(mutex, INFINITE);

	const size_t& _max2 = _inRenderDistanceChunks.size();
	for (size_t i = 0; i < _worldChunkSize; ++i)
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
	
	for (size_t i = 0; i < _worldChunkSize; ++i)
	{
		if (!worldChunks[i])
		{
			worldChunks.erase(worldChunks.begin() + i);
			i--;
			_worldChunkSize--;
		}
	}

	ReleaseMutex(mutex);
}

Chunk* Chunks_Manager::GetChunkAtPosition(const glm::vec3& _position) const
{
	WaitForSingleObject(mutex, INFINITE);

	const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk* _chunk = worldChunks[i];
		if (_position == _chunk->chunkPosition)
		{
			ReleaseMutex(mutex);
			return _chunk;
		}
	}
	ReleaseMutex(mutex);
	return nullptr;
}

bool Chunks_Manager::GetIsChunkAtPositionBeingGenerated(const glm::vec3& _position) const
{
	const size_t& _max = chunkPositionBeingGenerated.size();
	for (size_t i = 0; i < _max; ++i)
	{
		if (chunkPositionBeingGenerated[i] == _position)
			return true;
	}
	return false;
}