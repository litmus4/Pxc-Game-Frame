#pragma once

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

	static void SetReseedCycle(int iCycle);

private:
	static float RandBase();

private:
	static int s_iReseedCycle;
	static int s_iRandCount;
};

}