#include "PerlinNoise.h"

#include "lodepng.h"

#include <cmath>
#include <random>
#include <algorithm>
#include <numeric>



Perlin_Noise::Perlin_Noise()
{

}

Perlin_Noise::~Perlin_Noise()
{

}

void Perlin_Noise::Initialize()
{
	permutation = { 151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
		8,99,37,240,21,10,23,190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
		35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,
		134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
		55,46,245,40,244,102,143,54, 65,25,63,161,1,216,80,73,209,76,132,187,208, 89,
		18,169,200,196,135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,
		250,124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,
		189,28,42,223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167,
		43,172,9,129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,
		97,228,251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,
		107,49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
		138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };

	permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

void Perlin_Noise::GenerateImage()
{
	lodepng::State _state;

	for (size_t i = 0; i < 16; ++i)
	{
		unsigned char r = 127 * (1 + std::sin(5 * i * 6.28318531 / 16));
		unsigned char g = 127 * (1 + std::sin(2 * i * 6.28318531 / 16));
		unsigned char b = 127 * (1 + std::sin(3 * i * 6.28318531 / 16));
		unsigned char a = 63 * (1 + std::sin(8 * i * 6.28318531 / 16)) + 128;

		//lodepng_palette_add(&_state.info_png.color, r, g, b, a);
		lodepng_palette_add(&_state.info_raw, r, g, b, a);
	}

	//_state.info_png.color.colortype = LCT_PALETTE;
	_state.info_png.color.bitdepth = 4;
	_state.info_raw.colortype = LCT_PALETTE;
	_state.info_raw.bitdepth = 4;
	_state.encoder.auto_convert = 0;

	const unsigned w = 256, h = 256;
	std::vector<unsigned char> image;
	image.resize((w * h * 4 + 7) / 8, 0);
	for (size_t y = 0; y < h; ++y)
	{
		for (size_t x = 0; x < w; ++x)
		{
			size_t byte_index = (y * w + x) / 2;
			bool byte_half = (y * w + x) % 2 == 1;

			int color = (int)(4 * ((1 + std::sin(2.0 * 6.28318531 * x / (double)w))
				+ (1 + std::sin(2.0 * 6.28318531 * y / (double)h))));

			image[byte_index] |= (unsigned char)(color << (byte_half ? 0 : 4));
		}
	}

	std::vector<unsigned char> buffer;
	unsigned error = lodepng::encode(buffer, image.empty() ? 0 : &image[0], w, h, _state);
	if (error) 
	{
		throw std::exception("[Perlin_Noise::GenerateImage] -> invalid image generation");
	}
	
	
	
	//printf("%s", _filePath);

	//lodepng::save_file(buffer, );
}

void Perlin_Noise::SetSeed(unsigned seed)
{
	permutation.resize(256);

	std::iota(permutation.begin(), permutation.end(), 0);

	std::default_random_engine _engine(seed);

	std::shuffle(permutation.begin(), permutation.end(), _engine);

	permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

double Perlin_Noise::GetNoise(double x, double y, double z)
{
	int X = (int)floor(x) & 255;
	int Y = (int)floor(y) & 255;
	int Z = (int)floor(z) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	double u = Fade(x);
	double v = Fade(y);
	double w = Fade(z);

	int A = permutation[X] + Y;
	int AA = permutation[A] + Z;
	int AB = permutation[A + 1] + Z;
	int B = permutation[X + 1] + Y;
	int BA = permutation[B] + Z;
	int BB = permutation[B + 1] + Z;

	double res = Lerp(w, Lerp(v, Lerp(u, Grad(permutation[AA], x, y, z),
				 Grad(permutation[BA], x - 1, y, z)), Lerp(u, Grad(permutation[AB], x, y - 1, z),
				 Grad(permutation[BB], x - 1, y - 1, z))), Lerp(v, Lerp(u, Grad(permutation[AA + 1], x, y, z - 1),
				 Grad(permutation[BA + 1], x - 1, y, z - 1)), Lerp(u, Grad(permutation[AB + 1], x, y - 1, z - 1),
				 Grad(permutation[BB + 1], x - 1, y - 1, z - 1))));

	return (res + 1.0) / 2.0;
}

double Perlin_Noise::Fade(double t)
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double Perlin_Noise::Lerp(double t, double a, double b)
{
	return a + t * (b - a);
}

double Perlin_Noise::Grad(int hash, double x, double y, double z)
{
	int h = hash & 15;

	double u = h < 8 ? x : y,
		v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}