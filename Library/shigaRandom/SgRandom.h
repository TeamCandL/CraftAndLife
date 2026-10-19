#pragma once
#include <random>

class Random
{
public:
	static int GetRandom(int min, int max)
	{
		std::uniform_int_distribution<int> dist(min, max);
		return dist(GetIndex());
	}
	static int GetRandom(int max)
	{
		std::uniform_int_distribution<int> dist(0, max);
		return dist(GetIndex());
	}
	static bool GetHalf()
	{
		std::bernoulli_distribution dist(0.5);
		return dist(GetIndex());
	}

private:
	static std::mt19937 &GetIndex()
	{
		static std::random_device rd;
		static std::mt19937 index(rd());
		return index;
	}
};
