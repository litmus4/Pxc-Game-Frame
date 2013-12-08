#include "Random.h"
#include "Scattered.h"
#include <stdlib.h>
#include <time.h>
#include <math.h>

namespace PxcUtil
{

int Random::s_iReseedCycle = 0;
int Random::s_iRandCount = 0;

int Random::RandInt(int iMin, int iMax)
{
	if (iMin > iMax)
	{
		int iTmp = iMin;
		iMin = iMax;
		iMax = iTmp;
	}
	else if (iMin == iMax)
		return iMin;

	if ((iMax - iMin) > RAND_MAX)
	{
		int iTimes = RandInt(0, (iMax - iMin) / (RAND_MAX + 1));
		int iBlockMin = iMin + iTimes * (RAND_MAX + 1);
		if (iMax - iBlockMin < RAND_MAX)
			return RandInt(iBlockMin, iMax);
		else
			return RandInt(iBlockMin, iBlockMin + RAND_MAX);
	}
	else
		return iMin + (int)(RandBase() * (iMax - iMin + 1));
}

float Random::RandFloat(float fMin, float fMax)
{
	if (fMin > fMax)
	{
		float fTmp = fMin;
		fMin = fMax;
		fMax = fTmp;
	}
	else if (fabs(fMax - fMin) < FLOAT_PRECISION)
		return fMin;
	else if ((int)(fMax - fMin) > RAND_MAX)
		return 0.0f;

	float fUnitMin = fMin, fUnitMax = fMax;
	if ((fMax - fMin) > 1.0f)
	{
		int iTimes = RandInt(0, (int)(fMax - fMin));
		fUnitMin = fMin + (float)iTimes;
		if (fMax - fUnitMin >= 1.0f)
			fUnitMax = fUnitMin + 1.0f;
	}
	return fUnitMin + RandBase() * (fUnitMax - fUnitMin);
}

int Random::Rand_1To100()
{
	return RandInt(1, 100);
}

int Random::Rand_1To360(bool bZeroBase)
{
	if (bZeroBase)
		return RandInt(0, 359);
	else
		return RandInt(1, 360);
}

float Random::Rand_0To1_Float()
{
	return RandBase();
}

float Random::Rand_0To100_Float()
{
	return RandFloat(0.0f, 100.0f);
}

float Random::Rand_0To360_Float()
{
	return RandFloat(0.0f, 360.0f);
}

float Random::Rand_0To2PI_Float()
{
	return RandFloat(0.0f, 2.0f * PXCU_PI);
}

void Random::SetReseedCycle(int iCycle)
{
	s_iReseedCycle = iCycle;
	s_iRandCount = iCycle;
}

float Random::RandBase()
{
	if (s_iReseedCycle > 0)
	{
		if ((++s_iRandCount) >= s_iReseedCycle)
		{
			srand((unsigned int)time(NULL));
			s_iRandCount = 0;
		}
	}
	else
	{
		if (s_iRandCount <= 0)
		{
			srand((unsigned int)time(NULL));
			s_iRandCount = 1;
		}
	}
	return (float)rand() / (RAND_MAX + 1);
}

}