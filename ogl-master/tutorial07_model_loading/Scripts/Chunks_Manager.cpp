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
	mutex = CreateMutex(0, false, 0);

	chunkDataGenerator = new Chunk_Data_Generator(this);
	chunkRenderGenerator = new Chunk_Render_Generator(this);
	threadManager = &Thread_Manager::Instance();

	renderDistanceIndex = 1;
	renderDistance = Render_Distance_Current;
	renderMaxDistance = renderDistance - 1;
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
	onTick.RemoveDynamic(this, &Chunks_Manager::CheckUpdateChunkSideRender);

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

void Chunks_Manager::AddChunk(SThread_AddChunk_Ptr _data)
{	
	const glm::vec3& _position = *_data->position;
	Chunks_Manager* _thisPtr = _data->thisPtr;
	Thread* _thisThread = _data->thisThread;

	Chunk* _chunk = new Chunk(_thisPtr->chunkDataGenerator, _thisPtr->chunkRenderGenerator, _position);

	WaitForSingleObject(_thisPtr->mutex, INFINITE);
	_thisPtr->chunkWaitingForCGgen.push_back(_chunk);
	ReleaseMutex(_thisPtr->mutex);

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
	ReleaseMutex(mutex);

	renderDistanceIndex = 1;

	CheckGenerateNewChunkRender();
	CheckGenerateChunkPosition();
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

		chunkPositionFinishGeneration.push_back(_chunk->chunkPosition);

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

	const size_t& _max = chunkWaitingForGraphicalUpdate.size();
	for (size_t i = 0; i < _max; ++i)
	{
		chunkWaitingForGraphicalUpdate[0]->UpdateChunkSideRender();
	}

	chunkWaitingForGraphicalUpdate.clear();

	ReleaseMutex(mutex);
}

void Chunks_Manager::CheckRenderDistance()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8.f);
	const glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	size_t _worldChunkSize = worldChunks.size();
	std::vector<Chunk*> _inRenderDistanceChunks;

	if (CheckIfNoChunkLoaded(_worldChunkSize, _playerPositionChunkRelative))return;

	/*const size_t& _renderIndex = pow((renderDistanceIndex * 2) - 1, 2);	//TODO calcultate for 3D
	const size_t& _worldSlicedSize = (_worldChunkSize - 1) / 2;

	if (_worldSlicedSize < _renderIndex)
	{
		// pas assez de chunk donc on doit en générer
		if (_worldChunkSize == 1)
			if (renderDistanceIndex < Render_Distance_Current)
				++renderDistanceIndex;
	}
	else if (_worldSlicedSize > _renderIndex)
	{
		// trop de chunk donc on doit d'abord delete ceux hors range
		--renderDistanceIndex;
	}
	else // (_worldSlicedSize == _renderIndex)
	{
		if (renderDistanceIndex < Render_Distance_Current)
			++renderDistanceIndex;
	}

	const int& _renderMaxDistance = renderDistanceIndex + 1;*/
	
	glm::vec3 _offset;
	for (int x = -renderMaxDistance; x < renderDistance; ++x)
	{
		_offset.x = x;

		for (int z = -renderMaxDistance; z < renderDistance; ++z)
		{
			/*if (abs(x) + abs(z) > renderDistanceIndex - 1)
			{
				// out of distance
				continue;
			}*/

			_offset.z = z;

			const glm::vec3& _chunkCheckPosition = _offset + _playerPositionChunkRelative;

			if (Chunk* _chunk = GetChunkAtPosition(_chunkCheckPosition))
			{
				_inRenderDistanceChunks.push_back(_chunk);
			}
			else if (!GetIsChunkAtPositionBeingGenerated(_chunkCheckPosition))
			{
				if (GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, -1, 0)) ||
					GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, 1, 0))  ||
					GetChunkAtPosition(_chunkCheckPosition + glm::vec3(-1, 0, 0)) ||
					GetChunkAtPosition(_chunkCheckPosition + glm::vec3(1, 0, 0))  ||
					GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, 0, -1)) ||
					GetChunkAtPosition(_chunkCheckPosition + glm::vec3(0, 0, 1)))
				{
					if (Thread* _thread = threadManager->CreateThread())
					{
						SThread_AddChunk_Ptr _data = new SThread_AddChunk();
						_data->thisThread = _thread;
						_data->thisPtr = this;
						_data->position = new glm::vec3(_chunkCheckPosition);
						
						chunkPositionBeingGenerated.push_back(_chunkCheckPosition);
						_thread->CreateThreadFunction(true, AddChunk, _data);
					}
				}
			}
		}
	}
	
	DeleteChunksOutOfRange(_inRenderDistanceChunks, _worldChunkSize);
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
				_data->position = new glm::vec3(_playerPositionChunkRelative);

				chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);
				_thread->CreateThreadFunction(true, AddChunk, _data);
			}
		}
		renderDistanceIndex = 1;
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
	const size_t& _max = chunkPositionBeingGenerated.size();
	for (size_t i = 0; i < _max; ++i)
	{
		if (chunkPositionBeingGenerated[i] == _position)
			return true;
	}
	return false;
}