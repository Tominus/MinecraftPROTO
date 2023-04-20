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
#include <array>


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

	opti_worldChunks = nullptr;
	opti_worldChunksOffset.x = opti_worldChunksOffset.z = -renderMaxDistance;
	opti_worldChunksCount = 0;
}

Chunks_Manager::~Chunks_Manager()
{
	do {
		//Wait thread to finish there behavior
	} while (threadManager->GetHasAllThreadFinished() == false);

	WaitForSingleObject(mutex, INFINITE);
	/*const size_t& _max = worldChunks.size();
	for (size_t i = 0; i < _max; ++i)
	{
		Chunk*& _chunk = worldChunks[i];
		delete _chunk;
		_chunk = nullptr;
	}*/

	if (opti_worldChunks)
	{
		for (size_t x = 0; x < Render_Distance_Total; ++x)
		{
			Chunk*** _chunksX = opti_worldChunks[x];

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Chunk** _chunksY = _chunksX[y];

				for (size_t z = 0; z < Render_Distance_Total; ++z)
					delete _chunksY[z];

				delete[] _chunksY;
			}
			delete[] _chunksX;
		}
		delete[] opti_worldChunks;
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
	//onTick.AddDynamic(this, &Chunks_Manager::CheckRenderDistance);
	onTick.AddDynamic(this, &Chunks_Manager::Opti_CheckRenderDistance);
	onTick.AddDynamic(this, &Chunks_Manager::CheckUpdateChunkSideRender);

	opti_worldChunks = new Chunk ***[Render_Distance_Total];
	for (size_t x = 0; x < Render_Distance_Total; ++x)
	{
		Chunk***& _chunksX = opti_worldChunks[x];
		_chunksX = new Chunk **[Chunk_Size];

		for (size_t y = 0; y < Chunk_Size; ++y)
		{
			Chunk**& _chunksY = _chunksX[y];
			_chunksY = new Chunk *[Render_Distance_Total];

			for (size_t z = 0; z < Render_Distance_Total; ++z)
			{
				_chunksY[z] = nullptr;
			}
		}
	}
}

Threaded void Chunks_Manager::AddChunk(SThread_AddChunk_Ptr _data)
{	
	Thread* _thisThread = _data->thisThread;
	Chunks_Manager* _thisPtr = _data->thisPtr;
	HANDLE _mutex = _thisPtr->mutex;

	WaitForSingleObject(_mutex, INFINITE);
	Chunk* _chunk = _data->chunk;
	ReleaseMutex(_mutex);

	//---Init
	_chunk->Init();

	//---Data
	_chunk->InitChunkData();

	WaitForSingleObject(_mutex, INFINITE);
	bool _hasRender = _chunk->chunkRender->bHasRender;
	Chunk_Data* _chunkData = _chunk->chunkData;

	if (!_thisPtr->bInterruptThread_NotSafe)
	{
		_thisPtr->chunkBeingGenerating.push_back(_chunk);
		_thisPtr->onChunkDestroyed.AddDynamic(_chunkData, &Chunk_Data::RemoveSideChunk);
		_thisPtr->chunkDataGenerator->SetSideChunks(_chunkData);
	}
	ReleaseMutex(_mutex);

	//---Side
	WaitForSingleObject(_mutex, INFINITE);
	bool _hasFinish = _chunkData->CheckChunkToWaitEmpty();
	ReleaseMutex(_mutex);

	while (_hasFinish == false)
	{
		Sleep(16);

		WaitForSingleObject(_mutex, INFINITE);
		_hasFinish = _chunkData->CheckChunkToWaitEmpty();
		if (_thisPtr->bInterruptThread_NotSafe)
		{
			_chunk->FinishInit();
			delete _chunk;
			delete _data;
			ReleaseMutex(_mutex);
			_thisThread->OnFinished.Invoke(_thisThread);
			return;
		}
		ReleaseMutex(_mutex);
	}

	if (_hasRender)
	{
		//---Render
		_chunk->InitChunkRender();
	}

	//---Finish
	_chunk->FinishInit();

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

Threaded void Chunks_Manager::TickChunksManager() const
{
	WaitForSingleObject(mutex, INFINITE);

	onTick.Invoke();

	ReleaseMutex(mutex);
}

void Chunks_Manager::UpdateRender()
{
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUseProgram(programID);

	const size_t& _max = worldChunksToRender.size();

	for (size_t i(0); i < _max; ++i)
		worldChunksToRender[i]->Render();

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

void Chunks_Manager::Exit()
{
	WaitForSingleObject(mutex, INFINITE);

	bInterruptThread_NotSafe = true;

	chunkBeingGenerating.clear();

	onChunkInitialized.Clear();
	onChunkDestroyed.Clear();

	ReleaseMutex(mutex);
}

void Chunks_Manager::CheckGenerateNewChunkRender()
{
	size_t _size (chunkWaitingForCGgen.size());
	size_t _index = 0;

	while (_size > 0)
	{
		Chunk* _chunk = chunkWaitingForCGgen[0];
		Chunk_Render* _chunkRender = _chunk->chunkRender;
		const bool& _hasRender = _chunkRender->bHasRender;

		if (_hasRender)
		{
			_chunk->GenerateCGRender();
		}
		_chunkRender->bHasFinishGeneration = true;

		chunkWaitingForCGgen.erase(chunkWaitingForCGgen.begin());
		--_size;

		chunkPositionFinishGeneration.push_back(_chunk->chunkPosition);
		for (_index = 0; chunkBeingGenerating[_index] != _chunk; ++_index)
		{

		}
		chunkBeingGenerating.erase(chunkBeingGenerating.begin() + _index);

		if (!Opti_AddChunk(_chunk))
		{
			//Put this chunk to the delete list
			delete _chunk;
		}
		else
		{
			worldChunks.push_back(_chunk);
			if (_hasRender)
				worldChunksToRender.push_back(_chunk);
		}
	}
}

void Chunks_Manager::CheckGenerateChunkPosition()
{
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
}

void Chunks_Manager::CheckUpdateChunkSideRender()
{
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
}

void Chunks_Manager::CheckRenderDistance()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8.f);
	const glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	size_t _worldChunkSize = worldChunks.size();
	std::vector<Chunk*> _inRenderDistanceChunks;

	std::vector<Thread*> _threadToActivate;

	if (CheckIfNoChunkLoaded(_worldChunkSize, _playerPositionChunkRelative))return;

	size_t _totalCreatedChunk = 0;
	
	glm::vec3 _offset;
	for (int y = Chunk_Max_World_Height - 1; y > Chunk_Min_World_Height; --y)
	{
		_offset.y = y;

		for (int x = -renderMaxDistance; x < renderDistance; ++x)
		{
			_offset.x = x;

			for (int z = -renderMaxDistance; z < renderDistance; ++z)
			{
				_offset.z = z;

				const glm::vec3& _chunkCheckPosition = _offset + _playerPositionChunkRelative;

				if (Chunk* _chunk = Opti_GetChunk(_chunkCheckPosition))
				{
					_inRenderDistanceChunks.push_back(_chunk);
				}
				else if (!GetIsChunkAtPositionBeingGenerated(_chunkCheckPosition))
				{
					if (_totalCreatedChunk > Thread_Max_ChunkCreation)continue;

					if (Opti_GetFirstChunkAroundPosition(_chunkCheckPosition))
					{
						if (Thread* _thread = threadManager->CreateThread())
						{
							SThread_AddChunk_Ptr _data = new SThread_AddChunk();
							_data->thisThread = _thread;
							_data->thisPtr = this;
							_data->chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _chunkCheckPosition);
							_threadToActivate.push_back(_thread);

							_thread->CreateThreadFunction(false, AddChunk, _data);
							chunkPositionBeingGenerated.push_back(_chunkCheckPosition);
							++_totalCreatedChunk;
						}
					}
				}
			}
		}
	}
	
	DeleteChunksOutOfRange(_inRenderDistanceChunks, _worldChunkSize);

	for (size_t i = 0; i < _totalCreatedChunk; ++i)
	{
		_threadToActivate[i]->Execute();
	}
}

bool Chunks_Manager::CheckIfNoChunkLoaded(const size_t& _worldChunkSize, glm::vec3 _playerPositionChunkRelative)
{
	if (_worldChunkSize == 0 || opti_worldChunks == nullptr)
	{
		if (chunkPositionBeingGenerated.size() == 0 && chunkPositionFinishGeneration.size() == 0)
		{
			float _playerHeight = round((getPositionHeight() - 8.f) / 16.f);
			if (_playerHeight < Chunk_Min_World_Height + 1)
			{
				_playerPositionChunkRelative.y = Chunk_Min_World_Height + 1;
			}
			else if (_playerHeight > Chunk_Max_World_Height - 1)
			{
				_playerPositionChunkRelative.y = Chunk_Max_World_Height - 1;
			}

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
	//Delete chunk that are not in range of view
	const size_t& _max2 = _chunkInRange.size();
	for (size_t i = 0; i < _worldChunkSize; ++i)
	{
		Chunk*& _chunk = worldChunks[i];

		bool _isInRange = false;
		for (size_t j = 0; j < _max2; ++j)
		{
			if (_chunk == _chunkInRange[j])
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
				if (chunkWaitingForGraphicalUpdate[k] == _chunk)
				{
					chunkWaitingForGraphicalUpdate.erase(chunkWaitingForGraphicalUpdate.begin() + k);
					break;
				}
			}

			if (_chunk->chunkRender->bHasRender)
			{
				const size_t& _worldRenderSize = worldChunksToRender.size();
				for (size_t k = 0; k < _worldRenderSize; ++k)
				{
					if (_chunk == worldChunksToRender[k])
					{
						worldChunksToRender.erase(worldChunksToRender.begin() + k);
						break;
					}
				}
			}


			onChunkDestroyed.Invoke_Delete(_chunk);

			delete _chunk;
			_chunk = nullptr;
		}
	}

	//Clear deleted chunk data
	for (size_t i = 0; i < _worldChunkSize; ++i)
	{
		if (!worldChunks[i])
		{
			worldChunks.erase(worldChunks.begin() + i);
			--i;
			--_worldChunkSize;
		}
	}
}

void Chunks_Manager::Opti_CheckRenderDistance()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8.f);
	glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	std::vector<Thread*> _threadToActivate;
	size_t _totalCreatedChunk = 0;

	if (Opti_CheckIfNoChunkLoaded(_playerPositionChunkRelative))return;

	const glm::vec3& _worldMiddleChunkPosition = opti_worldChunksOffset + glm::vec3(renderMaxDistance, 0, renderMaxDistance);

	if (_playerPositionChunkRelative.x != _worldMiddleChunkPosition.x || _playerPositionChunkRelative.z != _worldMiddleChunkPosition.z)
	{
		//Player not in middle of array

		const glm::ivec3& _moveOffset = _playerPositionChunkRelative - _worldMiddleChunkPosition;
		const float& _moveOffsetMinX = _moveOffset.x;
		const float& _moveOffsetMinZ = _moveOffset.z;
		const float& _moveOffsetMaxX = Render_Distance_Total + _moveOffsetMinX;
		const float& _moveOffsetMaxZ = Render_Distance_Total + _moveOffsetMinZ;

		//Delete chunk low
		for (size_t x = 0; x < Render_Distance_Total; ++x)
		{
			Chunk*** _chunksX = opti_worldChunks[x];

			for (size_t y = 0; y < Chunk_Size; ++y)
			{
				Chunk** _chunksY = _chunksX[y];

				for (size_t z = 0; z < Render_Distance_Total; ++z)
				{
					if (_moveOffsetMinX > x || _moveOffsetMinZ > z || _moveOffsetMaxX < x || _moveOffsetMaxZ < z)
					{
						if (Chunk*& _chunk = _chunksY[z])
						{
							--opti_worldChunksCount;
							delete _chunk;
							_chunk = nullptr;
						}
					}
				}
			}
		}


		//Move every chunks



		opti_worldChunksOffset.x += _moveOffsetMinX;
		opti_worldChunksOffset.z += _moveOffsetMinZ;
	}

	for (int y = Chunk_Max_Limit_World_Height; y > Chunk_Min_World_Height; --y)
	{
		_playerPositionChunkRelative.y += y;

		for (int x = -renderMaxDistance; x < renderDistance; ++x)
		{
			_playerPositionChunkRelative.x += x;

			for (int z = -renderMaxDistance; z < renderDistance; ++z)
			{
				_playerPositionChunkRelative.z += z;

				if (!Opti_GetChunk(_playerPositionChunkRelative))
				{
					if (!GetIsChunkAtPositionBeingGenerated(_playerPositionChunkRelative))
					{
						if (Opti_GetFirstChunkAroundPosition(_playerPositionChunkRelative))
						{
							if (Thread* _thread = threadManager->CreateThread())
							{
								SThread_AddChunk_Ptr _data = new SThread_AddChunk();
								_data->thisThread = _thread;
								_data->thisPtr = this;
								_data->chunk = new Chunk(chunkDataGenerator, chunkRenderGenerator, _playerPositionChunkRelative);
								_threadToActivate.push_back(_thread);

								_thread->CreateThreadFunction(false, AddChunk, _data);
								chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);
								++_totalCreatedChunk;

								if (_totalCreatedChunk > Thread_Max_ChunkCreation)
								{
									y = Chunk_Min_World_Height;
									x = renderDistance;
									break;
								}
							}
						}
					}
				}

				_playerPositionChunkRelative.z -= z;
			}

			_playerPositionChunkRelative.x -= x;
		}

		_playerPositionChunkRelative.y -= y;
	}


	for (size_t i = 0; i < _totalCreatedChunk; ++i)
	{
		_threadToActivate[i]->Execute();
	}
}

bool Chunks_Manager::Opti_CheckIfNoChunkLoaded(glm::vec3 _playerPositionChunkRelative)
{
	if (opti_worldChunksCount == 0)
	{
		if (chunkPositionBeingGenerated.size() == 0 && chunkPositionFinishGeneration.size() == 0)
		{
			const float& _playerHeight = round((getPositionHeight() - 8.f) / 16.f);
			if (_playerHeight < Chunk_Min_Limit_World_Height)
			{
				_playerPositionChunkRelative.y = Chunk_Min_Limit_World_Height;
			}
			else if (_playerHeight > Chunk_Max_Limit_World_Height)
			{
				_playerPositionChunkRelative.y = Chunk_Max_Limit_World_Height;
			}

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

bool Chunks_Manager::Opti_AddChunk(Chunk* _chunk)
{
	WaitForSingleObject(mutex, INFINITE);
	const glm::ivec3& _offset = _chunk->chunkPosition - opti_worldChunksOffset;
	
	if (_offset.x < 0 || _offset.x >= Render_Distance_Total || _offset.z < 0 || _offset.z >= Render_Distance_Total)
	{
		ReleaseMutex(mutex);
		return false;
	}
	
	opti_worldChunks[_offset.x][_offset.y][_offset.z] = _chunk;
	++opti_worldChunksCount;

	ReleaseMutex(mutex);
	return true;
}

Chunk* Chunks_Manager::Opti_GetChunk(const glm::vec3& _position)
{
	Chunk* _chunk = nullptr;

	WaitForSingleObject(mutex, INFINITE);
	const glm::ivec3& _offset = _position - opti_worldChunksOffset;

	if (_offset.x > -1 && _offset.x < Render_Distance_Total && _offset.z > -1 && _offset.z < Render_Distance_Total)
	{
		_chunk = opti_worldChunks[_offset.x][_offset.y][_offset.z];
	}

	ReleaseMutex(mutex);
	return _chunk;
}

Chunk* Chunks_Manager::Opti_GetFirstChunkAroundPosition(const glm::vec3& _position)
{
	WaitForSingleObject(mutex, INFINITE);
	const glm::ivec3& _offset = _position - opti_worldChunksOffset;

	if (_offset.x > -2 && _offset.x <= Render_Distance_Total && _offset.z > -2 && _offset.z <= Render_Distance_Total)
	{

		if (_offset.x > 0)
		{
			if (Chunk* _chunk = opti_worldChunks[_offset.x - 1][_offset.y][_offset.z])
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		if (_offset.y > 0)
		{
			if (Chunk* _chunk = opti_worldChunks[_offset.x][_offset.y - 1][_offset.z])
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		if (_offset.z > 0)
		{
			if (Chunk* _chunk = opti_worldChunks[_offset.x][_offset.y][_offset.z - 1])
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		if (_offset.x < Render_Distance_Total_Limit)
		{
			if (Chunk* _chunk = opti_worldChunks[_offset.x + 1][_offset.y][_offset.z])
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		if (_offset.y < Chunk_Max_Limit_World_Height)
		{
			if (Chunk* _chunk = opti_worldChunks[_offset.x][_offset.y + 1][_offset.z])
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}
		if (_offset.z < Render_Distance_Total_Limit)
		{
			if (Chunk* _chunk = opti_worldChunks[_offset.x][_offset.y][_offset.z + 1])
			{
				ReleaseMutex(mutex);
				return _chunk;
			}
		}

	}

	ReleaseMutex(mutex);
	return nullptr;
}