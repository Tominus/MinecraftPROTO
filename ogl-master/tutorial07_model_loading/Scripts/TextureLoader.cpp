#include "TextureLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TextureLoader::TextureLoader()
{

}

TextureLoader::~TextureLoader()
{

}

void TextureLoader::LoadTextures()
{
	//---
	blockTextureID.emplace(EBlock_Type::Air, LoadFile_PNG("Textures/block/acacia_log.png")); // TODO remove
	blockTextureID.emplace(EBlock_Type::Acacia_Log, LoadFile_PNG("Textures/block/acacia_log.png"));
	blockTextureID.emplace(EBlock_Type::Amethyst_Block, LoadFile_PNG("Textures/block/amethyst_block.png"));
	blockTextureID.emplace(EBlock_Type::Barrel_Side, LoadFile_PNG("Textures/block/barrel_side.png"));
	blockTextureID.emplace(EBlock_Type::Bee_Nest_Top, LoadFile_PNG("Textures/block/bee_nest_top.png"));
	blockTextureID.emplace(EBlock_Type::Deepslate_Diamond_Ore, LoadFile_PNG("Textures/block/deepslate_diamond_ore.png"));

	//---
	blockTextureID.emplace(EBlock_Type::Glass, LoadFile_PNG("Textures/block/glass.png"));
	blockTextureID.emplace(EBlock_Type::Ice, LoadFile_PNG("Textures/block/ice.png"));
	blockTextureID.emplace(EBlock_Type::Lime_Stained_Glass, LoadFile_PNG("Textures/block/lime_stained_glass.png"));
	blockTextureID.emplace(EBlock_Type::Red_Stained_Glass, LoadFile_PNG("Textures/block/red_stained_glass.png"));
}

GLuint TextureLoader::LoadFile_PNG(const char* _path)
{
	std::vector<unsigned char> _image;
	unsigned int _width, _height;
	
	if (lodepng::decode(_image, _width, _height, _path) != 0)
	{
		printf("TextureLoader::LoadFile_PNG -> Invalid png file");
		return 0;
	}

	GLuint _textureID;
	glGenTextures(1, &_textureID);

	glBindTexture(GL_TEXTURE_2D, _textureID);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _width, _height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &_image[0]);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	return _textureID;
}

const GLuint& TextureLoader::GetBlockTextureID(const EBlock_Type& _blockType) const
{
	return blockTextureID.at(_blockType);
}