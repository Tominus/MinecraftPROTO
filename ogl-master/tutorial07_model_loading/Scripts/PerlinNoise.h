#pragma once
#include <vector>

class Perlin_Noise
{

	friend class World;

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
	void Initialize();

	void GenerateImage();

	void SetSeed(unsigned seed);
	double GetNoise(double x, double y, double z);

	double Fade(double t);
	double Lerp(double t, double a, double b);
	double Grad(int hash, double x, double y, double z);

private:
	std::vector<int> permutation;

};