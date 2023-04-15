#include "Chunks_Manager.h"

#include "World.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_Render.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"
#include <common/controls.hpp>
#include "Thread_Manager.h"
#include "Thread.h"
#include "Shaders_Manager.h"

#include <Windows.h>
#include <stdio.h>


Chunks_Manager::Chunks_Manager()
{
	programID = Shaders_Manager::Instance().GetProgramID();

	mutex = CreateMutex(0, false, 0);
	bInterruptThread_NotSafe = false;

	chunkDataGenerator = new Chunk_Data_Generator(this);
	chunkRenderGenerator = new Chunk_Render_Generator(this);
	threadManager = &Thread_Manager::Instance();

	renderDistance = Render_Distance_Current;
	renderMaxDistance = renderDistance - 1;
	lastPlayerPosition = glm::vec3();
}

Chunks_Manager::~Chunks_Manager()
{
	do {
		//Wait thread to finish there behavior
	} while (threadManager->GetHasAllThreadFinished() == false);

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

	onUpdate.Clear();
	onTick.Clear();
	onChunkInitialized.Clear();
	onChunkDestroyed.Clear();

	const size_t& _max2 = chunkWaitingForCGgen.size();
	for (size_t i = 0; i < _max2; ++i)
	{
		delete chunkWaitingForCGgen[i];
	}

	delete chunkDataGenerator;
	delete chunkRenderGenerator;
}

void Chunks_Manager::StartChunkManager()
{
	onUpdate.AddDynamic(this, &Chunks_Manager::UpdateRender);

	onTick.AddDynamic(this, &Chunks_Manager::CheckGenerateNewChunkRender);
	onTick.AddDynamic(this, &Chunks_Manager::CheckGenerateChunkPosition);
	onTick.AddDynamic(this, &Chunks_Manager::CheckRenderDistance);
	onTick.AddDynamic(this, &Chunks_Manager::CheckUpdateChunkSideRender); 
}

Threaded void Chunks_Manager::AddChunk(SThread_AddChunk_Ptr _data)
{	
	Thread* _thisThread = _data->thisThread;
	Chunks_Manager* _thisPtr = _data->thisPtr;
	HANDLE _mutex = _thisPtr->mutex;

	WaitForSingleObject(_mutex, INFINITE);
	Chunk* _chunk = _data->chunk;
	Chunk_Data* _chunkData = _chunk->chunkData;
	ReleaseMutex(_mutex);

	_chunk->InitChunkData();
	
	WaitForSingleObject(_mutex, INFINITE);
	_thisPtr->chunkBeingGenerating.push_back(_chunk);
	_thisPtr->onChunkDestroyed.AddDynamic(_chunkData, &Chunk_Data::RemoveSideChunk);
	_thisPtr->chunkDataGenerator->SetSideChunks(_chunkData);
	ReleaseMutex(_mutex);

	bool _hasFinish = false;
	while (_hasFinish == false)
	{
		WaitForSingleObject(_mutex, INFINITE);
		_hasFinish = _chunkData->CheckChunkToWaitEmpty();
		if (_thisPtr->bInterruptThread_NotSafe)
		{
			delete _chunk;
			delete _data;
			_thisThread->OnFinished.Invoke(_thisThread);
			ReleaseMutex(_mutex);
			return;
		}
		ReleaseMutex(_mutex);

		Sleep(16);
	}

	_chunk->InitChunkRender();

	WaitForSingleObject(_mutex, INFINITE);
	_thisPtr->chunkWaitingForCGgen.push_back(_chunk);
	_thisPtr->onChunkInitialized.Invoke_Delete(_chunk);
	ReleaseMutex(_mutex);

	_thisThread->OnFinished.Invoke(_thisThread);

	delete _data;
}

void Chunks_Manager::AddStartingWorldBaseChunk()
{
	/*const glm::vec3& _playerPosition = getPosition() - glm::vec3(8.f);
	const glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);

	Chunk* _chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _playerPositionChunkRelative);

	WaitForSingleObject(mutex, INFINITE);
	chunkWaitingForCGgen.push_back(_chunk);
	ReleaseMutex(mutex);

	renderDistanceIndex = 1;

	CheckGenerateNewChunkRender();
	CheckGenerateChunkPosition();*/
}

void Chunks_Manager::AddWaitingForSideUpdateChunk(Chunk* _chunk)
{
	WaitForSingleObject(mutex, INFINITE);

	const size_t& _max = chunkWaitingForGraphicalUpdate.size();
	for (size_t i = 0; i < _max; ++i)
	{
		if (chunkWaitingForGraphicalUpdate[i] == _chunk)
			return;
	}

	chunkWaitingForGraphicalUpdate.push_back(_chunk);

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

	glUseProgram(programID);
	
	const size_t& _max = worldChunks.size();

	for (size_t i(0); i < _max; ++i)
		worldChunks[i]->Render();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Chunks_Manager::Exit()
{
	WaitForSingleObject(mutex, INFINITE);

	bInterruptThread_NotSafe = true;

	onChunkInitialized.Clear();
	onChunkDestroyed.Clear();

	ReleaseMutex(mutex);
}

void Chunks_Manager::CheckGenerateNewChunkRender()
{
	WaitForSingleObject(mutex, INFINITE);

	size_t _size (chunkWaitingForCGgen.size());
	size_t _index = 0;

	while (_size > 0)
	{
		Chunk* _chunk = chunkWaitingForCGgen[0];
		_chunk->GenerateCGRender();

		chunkWaitingForCGgen.erase(chunkWaitingForCGgen.begin());
		--_size;

		chunkPositionFinishGeneration.push_back(_chunk->chunkPosition);

		worldChunks.push_back(_chunk);

		for (_index = 0; chunkBeingGenerating[_index] != _chunk; ++_index)
		{

		}
		chunkBeingGenerating.erase(chunkBeingGenerating.begin() + _index);
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

		for (_index = 0; chunkPositionBeingGenerated[_index] != _finishPosition; ++_index)
		{
			
		}

		chunkPositionBeingGenerated.erase(chunkPositionBeingGenerated.begin() + _index);
		chunkPositionFinishGeneration.erase(chunkPositionFinishGeneration.begin());
		--_size;
	}
	ReleaseMutex(mutex);
}

void Chunks_Manager::CheckUpdateChunkSideRender()
{
	WaitForSingleObject(mutex, INFINITE);

	size_t _max = chunkWaitingForGraphicalUpdate.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk* _chunk = chunkWaitingForGraphicalUpdate[i];

		if (_chunk->chunkRender->bHasFinishGeneration)
		{
			_chunk->UpdateChunkSideRender();

			chunkWaitingForGraphicalUpdate.erase(chunkWaitingForGraphicalUpdate.begin() + i);
			--i;
			--_max;
		}
	}

	ReleaseMutex(mutex);
}

void Chunks_Manager::CheckRenderDistance()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8.f);
	const glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	size_t _worldChunkSize = worldChunks.size();
	std::vector<Chunk*> _inRenderDistanceChunks;

	std::vector<Thread*> _threadToActivate;

	if (CheckIfNoChunkLoaded(_worldChunkSize, _playerPositionChunkRelative))return;
	
	glm::vec3 _offset;
	//for (int y = Chunk_Zero_World_Height; y < Chunk_Max_World_Height; ++y)
	for (int y = Chunk_Max_World_Height - 1; y > Chunk_Zero_World_Height - 1; --y)
	{
		_offset.y = y;

		for (int x = -renderMaxDistance; x < renderDistance; ++x)
		{
			_offset.x = x;

			for (int z = -renderMaxDistance; z < renderDistance; ++z)
			{
				_offset.z = z;

				const glm::vec3& _chunkCheckPosition = _offset + _playerPositionChunkRelative;

				if (Chunk* _chunk = GetChunkAtPosition(_chunkCheckPosition))
				{
					_inRenderDistanceChunks.push_back(_chunk);
				}
				else if (!GetIsChunkAtPositionBeingGenerated(_chunkCheckPosition))
				{
					if (GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, -1, 0)) ||
						GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, 1, 0)) ||
						GetChunkAtPosition(_chunkCheckPosition + glm::vec3(-1, 0, 0)) ||
						GetChunkAtPosition(_chunkCheckPosition + glm::vec3(1, 0, 0)) ||
						GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, 0, -1)) ||
						GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, 0, 1)))
					{

						//if (_threadToActivate.size() > 1 || chunkBeingGenerating.size() > 1 || chunkPositionBeingGenerated.size() > 1 || chunkPositionFinishGeneration.size() > 1)
						{                                                                                                    ////TODO delete
						//	continue;
						}

						if (Thread* _thread = threadManager->CreateThread())
						{
							SThread_AddChunk_Ptr _data = new SThread_AddChunk();
							_data->thisThread = _thread;
							_data->thisPtr = this;
							_data->chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _chunkCheckPosition);
							_threadToActivate.push_back(_thread);

							_thread->CreateThreadFunction(false, AddChunk, _data);
							chunkPositionBeingGenerated.push_back(_chunkCheckPosition);
						}
					}
				}
			}
		}
	}
	
	DeleteChunksOutOfRange(_inRenderDistanceChunks, _worldChunkSize);

	const size_t& _size = _threadToActivate.size();
	for (size_t i = 0; i < _size; ++i)
	{
		_threadToActivate[i]->Execute();
	}
}

bool Chunks_Manager::CheckIfNoChunkLoaded(const size_t& _worldChunkSize, const glm::vec3& _playerPositionChunkRelative)
{
	if (_worldChunkSize == 0)
	{
		if (chunkPositionBeingGenerated.size() == 0 || chunkPositionFinishGeneration.size() == 0)
		{
			if (Thread* _thread = threadManager->CreateThread())
			{
				SThread_AddChunk_Ptr _data = new SThread_AddChunk();
				_data->thisThread = _thread;
				_data->thisPtr = this;
				chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);

				_data->chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _playerPositionChunkRelative);
				_thread->CreateThreadFunction(true, AddChunk, _data);
			}
		}
		return true;
	}
	return false;
}

void Chunks_Manager::DeleteChunksOutOfRange(std::vector<Chunk*>& _chunkInRange, size_t _worldChunkSize)
{
	WaitForSingleObject(mutex, INFINITE);

	//Delete chunk that are not in range of view
	const size_t& _max2 = _chunkInRange.size();
	for (size_t i = 0; i < _worldChunkSize; ++i)
	{
		Chunk*& _worldChunk = worldChunks[i];

		bool _isInRange = false;
		for (size_t j = 0; j < _max2; ++j)
		{
			if (_worldChunk == _chunkInRange[j])
			{
				_isInRange = true;
				break;
			}
		}

		if (!_isInRange)
		{
			const size_t& _size = chunkWaitingForGraphicalUpdate.size();
			for (size_t k = 0; k < _size; ++k)
			{
				if (chunkWaitingForGraphicalUpdate[k] == _worldChunk)
				{
					chunkWaitingForGraphicalUpdate.erase(chunkWaitingForGraphicalUpdate.begin() + k);
					break;
				}
			}

			onChunkDestroyed.Invoke_Delete(_worldChunk);

			delete _worldChunk;
			_worldChunk = nullptr;
		}
	}

	//Clear deleted chunk data
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
	WaitForSingleObject(mutex, INFINITE);

	const size_t& _max = chunkPositionBeingGenerated.size();
	for (size_t i = 0; i < _max; ++i)
	{
		if (chunkPositionBeingGenerated[i] == _position)
		{
			ReleaseMutex(mutex);
			return true;
		}
	}
	ReleaseMutex(mutex);
	return false;
}

Chunk* Chunks_Manager::GetChunkBeingGeneratedAtPosition(const glm::vec3& _position) const
{
	WaitForSingleObject(mutex, INFINITE);

	const size_t& _max = chunkBeingGenerating.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk* _chunk = chunkBeingGenerating[i];
		if (_position == _chunk->chunkPosition)
		{
			ReleaseMutex(mutex);
			return _chunk;
		}
	}
	ReleaseMutex(mutex);
	return nullptr;
}