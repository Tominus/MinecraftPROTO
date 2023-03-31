#pragma once

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

public:


};