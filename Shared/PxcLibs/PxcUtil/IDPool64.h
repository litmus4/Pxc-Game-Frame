#pragma once
#include <set>

namespace PxcUtil
{

class CIDPool64
{
public:
	CIDPool64(long long lMin, long long lMax, long long lInvalid);
	~CIDPool64();

	long long Generate();
	void Free(long long lID);
	void Declare(long long lID);

private:
	long long m_lMin;
	long long m_lMax;
	long long m_lInvalid;
	long long m_lCurCount;
	std::set<long long> m_setUsed;
	std::set<long long> m_setFree;
};

}