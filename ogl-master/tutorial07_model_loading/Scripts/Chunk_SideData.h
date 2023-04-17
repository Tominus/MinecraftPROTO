#pragma once

class Block;

class Chunk_SideData
{
	friend class Chunk;
	friend class Chunk_Data_Generator;
	friend class Chunk_Render_Generator;

private:
	Chunk_SideData();
	~Chunk_SideData();

private:
	Block*** downBlocks;
	Block*** upBlocks;
	Block*** leftBlocks;
	Block*** rightBlocks;
	Block*** backBlocks;
	Block*** frontBlocks;
};