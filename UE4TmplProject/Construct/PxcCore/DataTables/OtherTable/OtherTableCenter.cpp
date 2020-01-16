#include "OtherTableCenter.h"

COtherTableCenter::COtherTableCenter()
{
	//
}

COtherTableCenter::~COtherTableCenter()
{
	//
}

bool COtherTableCenter::Init(const std::string& strPath)
{
	LOADTABLE(GlobalParam, strPath, "OtherTable", m_mapGlobalParams, m_iID)
	return true;
}

void COtherTableCenter::Release()
{
	UNLOADTABLE(GlobalParam, m_mapGlobalParams)
	DeleteInstance();
}

CGlobalParamRow* COtherTableCenter::GetGlobalParamRow(int iID)
{
	std::map<int, CGlobalParamRow*>::iterator iter = m_mapGlobalParams.find(iID);
	if (iter != m_mapGlobalParams.end())
		return iter->second;
	return NULL;
}