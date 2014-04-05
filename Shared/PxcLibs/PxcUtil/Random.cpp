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

	int iRange = iMax - iMin + 1;
	if (iRange > (RAND_MAX + 1))
	{
		int iMaxTime = iRange / (RAND_MAX + 1);
		int iBlock = iRange / (iMaxTime + 1);
		int iTimes = RandInt(0, iMaxTime);
		int iBlockMin = iMin + iTimes * iBlock;
		if (iTimes == iMaxTime || iMax - iBlockMin + 1 < iBlock)
			return RandInt(iBlockMin, iMax);
		else
			return RandInt(iBlockMin, iBlockMin + iBlock - 1);
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
	float fRange = fMax - fMin;
	if (fRange > 1.0f)
	{
		float fUnit = fRange / (int)ceil(fRange);
		int iTimes = RandInt(0, (int)fRange);
		fUnitMin = fMin + iTimes * fUnit;
		if (fMax - fUnitMin >= fUnit)
			fUnitMax = fUnitMin + fUnit;
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

int Random::DrawLots(std::vector<int>& vecLots)
{
	int iSize = vecLots.size();
	if (iSize == 0)
		return 0;
	int iIndex = RandInt(0, iSize - 1);
	return vecLots[iIndex];
}

int Random::DrawLots(std::vector<std::pair<int, float>>& vecLots)
{
	float fTotalProb = 0.0f;
	std::vector<std::pair<int, float>>::iterator iter = vecLots.begin();
	for (; iter != vecLots.end(); iter++)
	{
		if (iter->second >= 0.0f)
			fTotalProb += iter->second;
	}
	if (fTotalProb == 0.0f)
		return 0;

	float fRand = RandFloat(0.0f, fTotalProb);
	float fCurMin = 0.0f;
	for (iter = vecLots.begin(); iter != vecLots.end(); iter++)
	{
		if (iter->second >= 0.0f)
		{
			float fCurMax = fCurMin + iter->second;
			if (fRand >= fCurMin && fRand < fCurMax)
				return iter->first;
			fCurMin = fCurMax;
		}
	}
	return 0;
}

bool Random::DrawLots(std::vector<int>& vecLots, int iNum, std::vector<int>& vecResults)
{
	int iSize = vecLots.size();
	if (iNum >= iSize)
	{
		vecResults = vecLots;
		return !vecResults.empty();
	}

	std::vector<int> vecLocal = vecLots;
	for (int i = 0; i < iNum; ++i)
	{
		int iIndex = RandInt(0, iSize - 1);
		vecResults.push_back(vecLocal[iIndex]);
		std::vector<int>::iterator iter = vecLocal.begin() + iIndex;
		vecLocal.erase(iter);
		iSize--;
	}
	return !vecResults.empty();
}

bool Random::DrawLots(std::vector<std::pair<int, float>>& vecLots, int iNum, std::vector<int>& vecResults)
{
	int iSize = vecLots.size();
	if (iNum >= iSize)
	{
		for (int i = 0; i < iSize; ++i)
		{
			if (vecLots[i].second >= 0.0f)
				vecResults.push_back(vecLots[i].first);
		}
		return !vecResults.empty();
	}

	std::vector<std::pair<int, float>> vecLocal = vecLots;
	float fTotalProb = 0.0f;
	std::vector<std::pair<int, float>>::iterator iter = vecLocal.begin();
	for (; iter != vecLocal.end(); iter++)
	{
		if (iter->second >= 0.0f)
			fTotalProb += iter->second;
	}
	if (fTotalProb == 0.0f)
		return false;

	for (int i = 0; i < iNum; ++i)
	{
		if (fTotalProb <= 0.0f)
			break;

		float fRand = RandFloat(0.0f, fTotalProb);
		float fCurMin = 0.0f;
		for (iter = vecLocal.begin(); iter != vecLocal.end(); iter++)
		{
			if (iter->second >= 0.0f)
			{
				float fCurMax = fCurMin + iter->second;
				if (fRand >= fCurMin && fRand < fCurMax)
				{
					vecResults.push_back(iter->first);
					fTotalProb -= iter->second;
					vecLocal.erase(iter);
					iSize--;
					break;
				}
				fCurMin = fCurMax;
			}
		}
	}
	return !vecResults.empty();
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