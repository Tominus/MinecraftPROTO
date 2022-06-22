#include "TextureLoader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

TextureLoader::TextureLoader()
{

}

TextureLoader::~TextureLoader()
{

}

void TextureLoader::LoadTextures()
{
	blockTextureID.emplace(Block_Type::Acacia_Log, LoadFile_DDS("TexturesDDS/block/acacia_log.dds"));
	blockTextureID.emplace(Block_Type::Amethyst_Block, LoadFile_DDS("TexturesDDS/block/amethyst_block.dds"));
	blockTextureID.emplace(Block_Type::Barrel_Side, LoadFile_DDS("TexturesDDS/block/barrel_side.dds"));
	blockTextureID.emplace(Block_Type::Deepslate_Diamond_Ore, LoadFile_DDS("TexturesDDS/block/deepslate_diamond_ore.dds"));
	blockTextureID.emplace(Block_Type::Glass, LoadFile_DDS("TexturesDDS/block/glass.dds"));
}

GLuint TextureLoader::LoadFile_DDS(const char* _path)
{
	unsigned char _header[124];

	FILE* _fp = fopen(_path, "rb");
	if (_fp == NULL)
	{
		printf("%s could not be opened. Are you in the right directory ? Don't forget to read the FAQ !\n", _path); getchar();
		return 0;
	}
	
	/* verify the type of file */
	char _filecode[4];
	fread(_filecode, 1, 4, _fp);
	if (strncmp(_filecode, "DDS ", 4) != 0)
	{
		fclose(_fp);
		return 0;
	}

	/* get the surface desc */
	fread(&_header, 124, 1, _fp);

	unsigned int _height = *(unsigned int*)&(_header[8]);
	unsigned int _width = *(unsigned int*)&(_header[12]);
	unsigned int _linearSize = *(unsigned int*)&(_header[16]);
	unsigned int _mipMapCount = *(unsigned int*)&(_header[24]);
	unsigned int _fourCC = *(unsigned int*)&(_header[80]);


	unsigned char* _buffer;
	unsigned int _bufsize;
	/* how big is it going to be including all mipmaps? */
	_bufsize = _mipMapCount > 1 ? _linearSize * 2 : _linearSize;
	_buffer = (unsigned char*)malloc(_bufsize * sizeof(unsigned char));
	fread(_buffer, 1, _bufsize, _fp);
	/* close the file pointer */
	fclose(_fp);

	unsigned int _components = (_fourCC == FOURCC_DXT1) ? 3 : 4;
	unsigned int _format;
	switch (_fourCC)
	{
	case FOURCC_DXT1:
		_format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
		break;
	case FOURCC_DXT3:
		_format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
		break;
	case FOURCC_DXT5:
		_format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
		break;
	default:
		free(_buffer);
		return 0;
	}

	// Create one OpenGL texture
	GLuint _textureID;
	glGenTextures(1, &_textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	unsigned int _blockSize = (_format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int _offset = 0;

	/* load the mipmaps */
	for (unsigned int _level = 0; _level < _mipMapCount && (_width || _height); ++_level)
	{
		const unsigned int& _size = ((_width + 3) / 4) * ((_height + 3) / 4) * _blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, _level, _format, _width, _height,	0, _size, _buffer + _offset);

		_offset += _size;
		_width /= 2;
		_height /= 2;

		// Deal with Non-Power-Of-Two textures. This code is not included in the webpage to reduce clutter.
		if (_width < 1) _width = 1;
		if (_height < 1) _height = 1;
	}

	//Added for 16x texture without blending
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	free(_buffer);

	return _textureID;
}

const GLuint& TextureLoader::GetBlockTextureID(const Block_Type& _blockType) const
{
	return blockTextureID.at(_blockType);
}