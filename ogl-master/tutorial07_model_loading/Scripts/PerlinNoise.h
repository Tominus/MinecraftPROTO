#pragma once
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
	double CalculateNoise(double x, double y) const;

private:
	double CalculateOctave(double x, double y) const;
	double ClampNoiseResult(const double x) const;

	double GetNoise(double x, double y) const;
	double Fade(const double t) const;
	double Grad(const unsigned _hash, const double x, const double y, const double z) const;
	double Lerp(const double a, const double b, const double t) const;

private:
	std::array<unsigned, 256> permutation;
	std::default_random_engine randomEngine;
	unsigned seed;

};