#include "ATHRand.h"

#include <random>

#define ATH_PRIME_1 6947
#define ATH_PRIME_2 57329
#define ATH_PRIME_3 486377
#define ATH_PRIME_4 9360289
#define ATH_PRIME_5 38501663

std::mt19937 g_mTwister(1);

namespace ATHRandom
{
	void SeedRand(unsigned long _nSeed)
	{
		g_mTwister.seed(_nSeed);
	}

	int Rand(int _nMin, int _nMax)
	{
		std::uniform_int_distribution<int> distro(_nMin, _nMax);
		return distro(g_mTwister);
	}
	float Rand(float _fMin, float _fMax)
	{
		std::uniform_real_distribution<float> distro(_fMin, _fMax);
		return distro(g_mTwister);
	}

	void SeedFastRand(unsigned int _nSeed)
	{
		srand((unsigned int)_nSeed);
	}
	int FastRand(int _nMin, int _nMax)
	{
		int nBase = rand();
		return nBase % (_nMax - _nMin) + _nMin;
	}
	float FastRand(float _fMin, float _fMax)
	{
		int nBase = rand();
		return (_fMax - _fMin) * (nBase / (float)RAND_MAX) + _fMin;
	}
}

