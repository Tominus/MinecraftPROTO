#include "PerlinNoise.h"

#include "lodepng.h"
#include "GlobalDefine.h"

#include <cmath>
#include <numeric>

Perlin_Noise::Perlin_Noise()
{
	permutation = { 151,160,137,91,90,15,
				131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
				190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
				88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
				77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
				102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
				135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
				5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
				223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
				129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
				251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
				49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
				138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180 };
	seed = 0;
}

Perlin_Noise::~Perlin_Noise()
{

}

void Perlin_Noise::InitializeSeed(unsigned _seed)
{
	seed = _seed;
	randomEngine = std::default_random_engine(seed);

	std::iota(permutation.begin(), permutation.end(), 0);
	std::shuffle(permutation.begin(), permutation.end(), randomEngine);
}

void Perlin_Noise::GenerateImage()
{
	std::vector<unsigned char> _image;

	for (size_t x = 0; x < Perlin_Noise_Resolution; ++x)
	{
		for (size_t y = 0; y < Perlin_Noise_Resolution; ++y)
		{
			_image.push_back((unsigned char)(CalculateNoise(x, y)));
		}
	}

	std::vector<unsigned char> _buffer;
	if (lodepng::encode(_buffer, &_image[0], Perlin_Noise_Resolution, Perlin_Noise_Resolution, LCT_GREY, 8))
	{
		throw std::exception("[Perlin_Noise::GenerateImage] -> invalid noise encoding");
	}

	if (lodepng::save_file(_buffer, "Save/map1/Noise.png"))
	{
		throw std::exception("[Perlin_Noise::GenerateImage] -> invalid noise save");
	}
}

double Perlin_Noise::CalculateNoise(double x, double y)
{
	return ClampNoiseResult(CalculateOctave(x * Perlin_Noise_Frequency, y * Perlin_Noise_Frequency)) * Perlin_Noise_Resolution;
}

double Perlin_Noise::CalculateOctave(double x, double y)
{
	double _result = 0.0;
	double _amplitude = 1.0;

	for (int i = 0; i < Perlin_Noise_Octave; ++i)
	{
		_result += GetNoise(x, y) * _amplitude;
		x *= 2;
		y *= 2;
		_amplitude *= 0.5;
	}

	return _result;
}

double Perlin_Noise::ClampNoiseResult(const double x)
{
	if (x <= -1.0)
	{
		return 0.0;
	}
	else if (x > 1.0)
	{
		return 1.0;
	}

	return (x * 0.5 + 0.5);
}

double Perlin_Noise::GetNoise(double x, double y)
{
	const double _x = std::floor(x);
	const double _y = std::floor(y);
	const double _z = std::floor(Perlin_Noise_DefaultZ);

	const int ix = (int)_x & 255;
	const int iy = (int)_y & 255;
	const int iz = (int)_z & 255;

	const double fx = (x - _x);
	const double fy = (y - _y);
	const double fz = (Perlin_Noise_DefaultZ - _z);

	const double u = Fade(fx);
	const double v = Fade(fy);
	const double w = Fade(fz);

	const unsigned A = (permutation[ix & 255] + iy) & 255;
	const unsigned B = (permutation[(ix + 1) & 255] + iy) & 255;

	const unsigned AA = (permutation[A] + iz) & 255;
	const unsigned AB = (permutation[(A + 1) & 255] + iz) & 255;

	const unsigned BA = (permutation[B] + iz) & 255;
	const unsigned BB = (permutation[(B + 1) & 255] + iz) & 255;

	const double p0 = Grad(permutation[AA], fx, fy, fz);
	const double p1 = Grad(permutation[BA], fx - 1, fy, fz);
	const double p2 = Grad(permutation[AB], fx, fy - 1, fz);
	const double p3 = Grad(permutation[BB], fx - 1, fy - 1, fz);
	const double p4 = Grad(permutation[(AA + 1) & 255], fx, fy, fz - 1);
	const double p5 = Grad(permutation[(BA + 1) & 255], fx - 1, fy, fz - 1);
	const double p6 = Grad(permutation[(AB + 1) & 255], fx, fy - 1, fz - 1);
	const double p7 = Grad(permutation[(BB + 1) & 255], fx - 1, fy - 1, fz - 1);

	const double q0 = Lerp(p0, p1, u);
	const double q1 = Lerp(p2, p3, u);
	const double q2 = Lerp(p4, p5, u);
	const double q3 = Lerp(p6, p7, u);

	const double r0 = Lerp(q0, q1, v);
	const double r1 = Lerp(q2, q3, v);

	return Lerp(r0, r1, w);
}

double Perlin_Noise::Fade(const double t)
{
	return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

double Perlin_Noise::Grad(const unsigned _hash, const double x, const double y, const double z)
{
	const unsigned h = _hash & 15;
	const double u = h < 8 ? x : y;
	const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double Perlin_Noise::Lerp(const double a, const double b, const double t)
{
	return a + (b - a) * t;
}