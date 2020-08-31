#include "IDPool64.h"

namespace PxcUtil
{

CIDPool64::CIDPool64(long long lMin, long long lMax, long long lInvalid)
{
	if (lMin > lMax)
	{
		long long lTmp = lMin;
		lMin = lMax;
		lMax = lTmp;
	}
	else if (lMin == lMax)
		lMax++;

	m_lMin = lMin;
	m_lMax = lMax;
	m_lInvalid = lInvalid;
	if (lInvalid == lMin)
		m_lCurCount = lMin + 1LL;
	else
		m_lCurCount = lMin;
}

CIDPool64::~CIDPool64()
{
	m_setUsed.clear();
	m_setFree.clear();
}

long long CIDPool64::Generate()
{
	long long lRet = m_lCurCount;
	if (m_setFree.empty())
	{
		if (m_lCurCount > m_lMax)
			return m_lInvalid;
		while (m_setUsed.find(m_lCurCount) != m_setUsed.end()
			|| m_lCurCount == m_lInvalid)
			lRet = ++m_lCurCount;
		m_lCurCount++;
		if (m_lCurCount == m_lInvalid)
			m_lCurCount++;
	}
	else
	{
		lRet = *m_setFree.begin();
		m_setFree.erase(m_setFree.begin());
	}
	m_setUsed.insert(lRet);
	return lRet;
}

void CIDPool64::Free(long long lID)
{
	std::set<long long>::iterator iter = m_setUsed.find(lID);
	if (iter != m_setUsed.end())
	{
		m_setUsed.erase(iter);
		m_setFree.insert(lID);
	}
}

void CIDPool64::Declare(long long lID)
{
	std::set<long long>::iterator iter = m_setFree.find(lID);
	if (iter == m_setFree.end())
	{
		if (lID < m_lMin || lID > m_lMax || lID == m_lInvalid)
			return;
	}
	else
		m_setFree.erase(iter);
	m_setUsed.insert(lID);
}

}