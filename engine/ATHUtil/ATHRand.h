#ifndef ATHRAND_H
#define ATHRAND_H

namespace ATHRandom
{
	void SeedRand(unsigned long _nSeed);
	int Rand(int _nMin, int _nMax);
	float Rand(float _fMin, float _fMax);
}


#endif