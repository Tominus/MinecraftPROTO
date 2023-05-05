#include "Chunks_Manager.h"

#include "World.h"
#include "Chunk.h"
#include "Chunk_Data.h"
#include "Chunk_Render.h"
#include "Chunk_Data_Generator.h"
#include "Chunk_Render_Generator.h"
#include "Chunk_Pool_Manager.h"
#include <common/controls.hpp>
#include "Thread_Manager.h"
#include "Thread.h"
#include "Shaders_Manager.h"

Chunks_Manager::Chunks_Manager()
{
	programID = Shaders_Manager::Instance()->GetProgramID();
	
	mutex = CreateMutex(0, false, 0);
	bInterruptThread_NotSafe = false;

	chunkDataGenerator = new Chunk_Data_Generator(this);
	chunkRenderGenerator = new Chunk_Render_Generator(this);
	chunkPoolManager = new Chunk_Pool_Manager(this, chunkDataGenerator, chunkRenderGenerator);
	threadManager = Thread_Manager::Instance();

	renderDistance = Render_Distance_Current;
	renderMaxDistance = renderDistance - 1;

	opti_worldChunks = nullptr;
	opti_worldChunksOffset.x = opti_worldChunksOffset.z = -(float)renderMaxDistance;
	opti_worldChunksCount = 0;
	opti_threadCount = 0;
}

Chunks_Manager::~Chunks_Manager()
{
	do {
		//Wait thread to finish there behavior
	} while (threadManager->GetHasAllThreadFinished() == false);

	WaitForSingleObject(mutex, INFINITE);

	const size_t& _size = worldChunksToDelete.size();
	for (size_t i = 0; i < _size; ++i)
	{
		delete worldChunksToDelete[i];
	}

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

	chunkPoolManager->DestroyAllChunkData();
	delete chunkPoolManager;
}

void Chunks_Manager::StartChunkManager()
{
	onUpdate.AddDynamic(this, &Chunks_Manager::UpdateRender);
	onUpdate.AddDynamic(this, &Chunks_Manager::DeleteChunks);

	onTick.AddDynamic(this, &Chunks_Manager::CheckGenerateNewChunkRender);
	onTick.AddDynamic(this, &Chunks_Manager::CheckGenerateChunkPosition);
	//onTick.AddDynamic(this, &Chunks_Manager::Opti_CheckRenderDistance);
	onTick.AddDynamic(this, &Chunks_Manager::CheckUpdateChunkSideRender);

	chunkPoolManager->InitializeAllChunkData();
	InitWorldChunksArray();
}

void Chunks_Manager::InitWorldChunksArray()
{
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

Threaded int __stdcall Chunks_Manager::AddChunk(SThread_AddChunk_Ptr _data)
{
	Thread* _thisThread = _data->thisThread;
	Chunks_Manager* _thisPtr = _data->chunkManager;
	HANDLE _mutex = _thisPtr->mutex;

	Chunk* _chunk = _thisPtr->chunkPoolManager->GetChunk();

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
		Sleep(4);

		WaitForSingleObject(_mutex, INFINITE);
		_hasFinish = _chunkData->CheckChunkToWaitEmpty();
		if (_thisPtr->bInterruptThread_NotSafe)
		{
			_chunk->FinishInit();
			delete _chunk;
			ReleaseMutex(_mutex);
			_thisThread->OnFinished.Invoke(_thisThread);
			return 2;
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
	return 1;
}

void Chunks_Manager::DeleteChunks()
{
	const size_t& _size = worldChunksToDelete.size();
	if (_size)
	{
		delete worldChunksToDelete[0];
		worldChunksToDelete.erase(worldChunksToDelete.begin());
	}
}

void Chunks_Manager::AddStartingWorldBaseChunk()
{

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
		_chunk->DeleteHandle();

		Chunk_Render* _chunkRender = _chunk->chunkRender;
		const bool& _hasRender = _chunkRender->bHasRender;
		--opti_threadCount;

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

		if (Opti_AddChunk(_chunk))
		{
			if (_hasRender)
				worldChunksToRender.push_back(_chunk);
		}
		else
		{
			//Put this chunk to the delete list
			_chunk->PreDeleteChunk();
			worldChunksToDelete.push_back(_chunk);
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

void Chunks_Manager::Opti_CheckRenderDistance()
{
	const glm::vec3& _playerPosition = getPosition() - glm::vec3(8.f);
	glm::vec3 _playerPositionChunkRelative(round(_playerPosition.x / 16.f), 0.f, round(_playerPosition.z / 16.f));

	Opti_RecenterWorldChunksArray(_playerPositionChunkRelative);

	if (Opti_CheckIfNoChunkLoaded(_playerPositionChunkRelative))return;

	std::vector<Thread*> _threadToActivate;
	size_t _totalCreatedChunk = 0;

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
								_threadToActivate.push_back(_thread);

								_thread->CreateThreadFunction(false, AddChunk, nullptr);
								chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);
								++_totalCreatedChunk;
								++opti_threadCount;

								if (_totalCreatedChunk > Thread_Max_ChunkCreation || opti_threadCount > Thread_Max_ChunkCreation)
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

void Chunks_Manager::Opti_RecenterWorldChunksArray(const glm::vec3& _playerPositionChunkRelative)
{
	const glm::vec3& _worldMiddleChunkPosition = opti_worldChunksOffset + glm::vec3(renderMaxDistance, 0, renderMaxDistance);

	if (_playerPositionChunkRelative.x != _worldMiddleChunkPosition.x || _playerPositionChunkRelative.z != _worldMiddleChunkPosition.z)
	{
		//Player not in middle of array

		const glm::ivec3& _moveOffset = _playerPositionChunkRelative - _worldMiddleChunkPosition;
		const int& _moveOffsetMinX = _moveOffset.x;
		const int& _moveOffsetMinZ = _moveOffset.z;
		const int& _moveOffsetMaxX = Render_Distance_Total + _moveOffsetMinX - 1;
		const int& _moveOffsetMaxZ = Render_Distance_Total + _moveOffsetMinZ - 1;

		//Delete chunk low
		for (int x = 0; x < Render_Distance_Total; ++x)
		{
			Chunk*** _chunksX = opti_worldChunks[x];

			for (int y = 0; y < Chunk_Max_World_Height; ++y)
			{
				Chunk** _chunksY = _chunksX[y];

				for (int z = 0; z < Render_Distance_Total; ++z)
				{
					if (_moveOffsetMinX > x || _moveOffsetMinZ > z || _moveOffsetMaxX < x || _moveOffsetMaxZ < z)
					{
						if (Chunk*& _chunk = _chunksY[z])
						{
							--opti_worldChunksCount;

							if (_chunk->chunkRender->bHasRender)
							{
								const size_t& _chunkToRenderSize = worldChunksToRender.size();
								for (size_t i = 0; i < _chunkToRenderSize; ++i)
								{
									if (worldChunksToRender[i] == _chunk)
									{
										worldChunksToRender.erase(worldChunksToRender.begin() + i);
										break;
									}
								}
							}

							_chunk->PreDeleteChunk();
							worldChunksToDelete.push_back(_chunk);
							_chunk = nullptr;
						}
					}
				}
			}
		}

		//Move every chunks
		const bool& _isXMin = _moveOffsetMinX > -1;
		const int& _toAddX = _isXMin ? 1 : -1;
		const int& _xStartOffset = _isXMin ? _moveOffsetMinX : _moveOffsetMaxX;
		const bool& _isZMin = _moveOffsetMinZ > -1;
		const int& _toAddZ = _isZMin ? 1 : -1;
		const int& _zStartOffset = _isZMin ? _moveOffsetMinZ : _moveOffsetMaxZ;

		for (int x = _xStartOffset; x < Render_Distance_Total && x > -1; x += _toAddX)
		{
			for (int y = 0; y < Chunk_Max_World_Height; ++y)
			{
				for (int z = _zStartOffset; z < Render_Distance_Total && z > -1; z += _toAddZ)
				{
					Chunk*& _chunk = opti_worldChunks[x][y][z];
					opti_worldChunks[x - _moveOffsetMinX][y][z - _moveOffsetMinZ] = _chunk;
					_chunk = nullptr;
				}
			}
		}

		opti_worldChunksOffset.x += _moveOffsetMinX;
		opti_worldChunksOffset.z += _moveOffsetMinZ;
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
				++opti_threadCount;
				chunkPositionBeingGenerated.push_back(_playerPositionChunkRelative);

				_thread->CreateThreadFunction(true, AddChunk, nullptr);
			}
		}
		return true;
	}
	return false;
}