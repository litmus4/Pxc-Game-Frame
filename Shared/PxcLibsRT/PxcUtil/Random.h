#pragma once
#include <vector>

namespace PxcUtil
{

class Random
{
public:
	static int RandInt(int iMin, int iMax);
	static float RandFloat(float fMin, float fMax);

	static int Rand_1To100();
	static int Rand_1To360(bool bZeroBase = false);
	static float Rand_0To1_Float();
	static float Rand_0To100_Float();
	static float Rand_0To360_Float();
	static float Rand_0To2PI_Float();

	static int DrawLots(std::vector<int>& vecLots);
	static int DrawLots(std::vector<std::pair<int, float>>& vecLots);
	static bool DrawLots(std::vector<int>& vecLots, int iNum, std::vector<int>& vecResults);
	static bool DrawLots(std::vector<std::pair<int, float>>& vecLots, int iNum, std::vector<int>& vecResults);

	static void SetReseedCycle(int iCycle);

private:
	static float RandBase();

private:
	static int s_iReseedCycle;
	static int s_iRandCount;
};

}