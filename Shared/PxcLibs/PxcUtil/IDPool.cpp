#include "IDPool.h"

namespace PxcUtil
{

CIDPool::CIDPool(int iMin, int iMax, int iInvalid)
{
	if (iMin > iMax)
	{
		int iTmp = iMin;
		iMin = iMax;
		iMax = iTmp;
	}
	else if (iMin == iMax)
		iMax++;

	m_iMin = iMin;
	m_iMax = iMax;
	m_iInvalid = iInvalid;
	if (iInvalid == iMin)
		m_iCurCount = iMin + 1;
	else
		m_iCurCount = iMin;
}

CIDPool::~CIDPool()
{
	m_setUsed.clear();
	m_setFree.clear();
}

int CIDPool::Generate()
{
	int iRet = m_iCurCount;
	if (m_setFree.empty())
	{
		if (m_iCurCount > m_iMax)
			return m_iInvalid;
		while (m_setUsed.find(m_iCurCount) != m_setUsed.end()
			|| m_iCurCount == m_iInvalid)
			iRet = ++m_iCurCount;
		m_iCurCount++;
		if (m_iCurCount == m_iInvalid)
			m_iCurCount++;
	}
	else
	{
		iRet = *m_setFree.begin();
		m_setFree.erase(m_setFree.begin());
	}
	m_setUsed.insert(iRet);
	return iRet;
}

void CIDPool::Free(int iID)
{
	std::set<int>::iterator iter = m_setUsed.find(iID);
	if (iter != m_setUsed.end())
	{
		m_setUsed.erase(iter);
		m_setFree.insert(iID);
	}
}

void CIDPool::Declare(int iID)
{
	std::set<int>::iterator iter = m_setFree.find(iID);
	if (iter == m_setFree.end())
	{
		if (iID < m_iMin || iID > m_iMax || iID == m_iInvalid)
			return;
	}
	else
		m_setFree.erase(iter);
	m_setUsed.insert(iID);
}

}