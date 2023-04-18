#pragma once
#include "GlobalDefine.h"

#include <array>
#include <random>
#include <Windows.h>

class Perlin_Noise
{

	friend class World;
	friend class Chunk_Data_Generator;

private:
	Perlin_Noise();
	~Perlin_Noise();

public:
	inline static Perlin_Noise& Instance()
	{
		static Perlin_Noise perlinNoise;
		return perlinNoise;
	}

private:
	void InitializeSeed(unsigned _seed);
	void GenerateImage();

public:
	inline double CalculateNoise(const double& x, const double& y) const
	{
		return ClampNoiseResult(CalculateOctave(x * Perlin_Noise_Frequency, y * Perlin_Noise_Frequency)) * Perlin_Noise_Resolution;
	}

private:
	inline double CalculateOctave(double x, double y) const
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

	inline double ClampNoiseResult(const double& x) const
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

	inline double GetNoise(const double& x, const double& y) const
	{
		const double& _x = std::floor(x);
		const double& _y = std::floor(y);
		const double& _z = std::floor(Perlin_Noise_DefaultZ);

		const int& ix = (int)_x & 255;
		const int& iy = (int)_y & 255;
		const int& iz = (int)_z & 255;

		const double& fx = (x - _x);
		const double& fy = (y - _y);
		const double& fz = (Perlin_Noise_DefaultZ - _z);

		const double& u = Fade(fx);
		const double& v = Fade(fy);
		const double& w = Fade(fz);

		const unsigned& A = (permutation[ix & 255] + iy) & 255;
		const unsigned& B = (permutation[(ix + 1) & 255] + iy) & 255;

		const unsigned& AA = (permutation[A] + iz) & 255;
		const unsigned& AB = (permutation[(A + 1) & 255] + iz) & 255;

		const unsigned& BA = (permutation[B] + iz) & 255;
		const unsigned& BB = (permutation[(B + 1) & 255] + iz) & 255;

		const double& p0 = Grad(permutation[AA], fx, fy, fz);
		const double& p1 = Grad(permutation[BA], fx - 1, fy, fz);
		const double& p2 = Grad(permutation[AB], fx, fy - 1, fz);
		const double& p3 = Grad(permutation[BB], fx - 1, fy - 1, fz);
		const double& p4 = Grad(permutation[(AA + 1) & 255], fx, fy, fz - 1);
		const double& p5 = Grad(permutation[(BA + 1) & 255], fx - 1, fy, fz - 1);
		const double& p6 = Grad(permutation[(AB + 1) & 255], fx, fy - 1, fz - 1);
		const double& p7 = Grad(permutation[(BB + 1) & 255], fx - 1, fy - 1, fz - 1);

		const double& q0 = Lerp(p0, p1, u);
		const double& q1 = Lerp(p2, p3, u);
		const double& q2 = Lerp(p4, p5, u);
		const double& q3 = Lerp(p6, p7, u);

		const double& r0 = Lerp(q0, q1, v);
		const double& r1 = Lerp(q2, q3, v);

		return Lerp(r0, r1, w);
	}

	inline double Fade(const double& t) const
	{
		return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
	}

	inline double Grad(const unsigned& _hash, const double& x, const double& y, const double& z) const
	{
		const unsigned& h = _hash & 15;
		const double& u = h < 8 ? x : y;
		const double& v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	inline double Lerp(const double& a, const double& b, const double& t) const
	{
		return a + (b - a) * t;
	}

private:
	std::array<unsigned, 256> permutation;
	std::default_random_engine randomEngine;
	unsigned seed;

};