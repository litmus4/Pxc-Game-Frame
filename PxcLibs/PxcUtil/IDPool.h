#pragma once
#include <set>

namespace PxcUtil
{

class CIDPool
{
public:
	CIDPool(int iMin, int iMax, int iInvalid);
	~CIDPool();

	int Generate();
	void Free(int iID);
	void Declare(int iID);

private:
	int m_iMin;
	int m_iMax;
	int m_iInvalid;
	int m_iCurCount;
	std::set<int> m_setUsed;
	std::set<int> m_setFree;
};

}