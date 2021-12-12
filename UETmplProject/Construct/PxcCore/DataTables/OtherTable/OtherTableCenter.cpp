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

	LOADTABLE(InputKey, strPath, "OtherTable", m_mapInputKeys, m_iID)
	{
		std::map<int, CInputKeyRow*>::iterator iter = m_mapInputKeys.begin();
		for (; iter != m_mapInputKeys.end(); iter++)
			m_mapInputKeyNames.insert(std::make_pair(iter->second->m_strKeyName, iter->first));
	}

	m_strPath = strPath;
	return true;
}

void COtherTableCenter::Release()
{
	UNLOADTABLE(GlobalParam, m_mapGlobalParams)

	UNLOADTABLE(InputActionName, m_mapInputActions)
	m_mapInputActionNames.clear();
	UNLOADTABLE(InputKey, m_mapInputKeys)
	m_mapInputKeyNames.clear();

	DeleteInstance();
}

CGlobalParamRow* COtherTableCenter::GetGlobalParamRow(int iID)
{
	std::map<int, CGlobalParamRow*>::iterator iter = m_mapGlobalParams.find(iID);
	if (iter != m_mapGlobalParams.end())
		return iter->second;
	return NULL;
}

std::string COtherTableCenter::LinkAxisName(const std::string& strAxisName, bool bPositiveDir)
{
	return (strAxisName + (bPositiveDir ? "1" : "0"));
}

bool COtherTableCenter::LoadInputActionNames()
{
	if (!m_mapInputActions.empty())
		return false;

	LOADTABLE(InputActionName, m_strPath, "OtherTable", m_mapInputActions, m_iID)
	{
		std::map<int, CInputActionNameRow*>::iterator iter = m_mapInputActions.begin();
		for (; iter != m_mapInputActions.end(); iter++)
		{
			std::string&& strUniActionName = (iter->second->m_bAxis ?
				LinkAxisName(iter->second->m_strActionName, iter->second->m_bAxisPositive) : iter->second->m_strActionName);
			m_mapInputActionNames.insert(std::make_pair(strUniActionName, iter->first));
		}
	}
	return true;
}

CInputActionNameRow* COtherTableCenter::GetInputActionNameRow(int iID)
{
	std::map<int, CInputActionNameRow*>::iterator iter = m_mapInputActions.find(iID);
	if (iter != m_mapInputActions.end())
		return iter->second;
	return NULL;
}

CInputActionNameRow* COtherTableCenter::GetInputActionNameRowByName(const std::string& strActionName)
{
	std::map<std::string, int>::iterator itName = m_mapInputActionNames.find(strActionName);
	if (itName != m_mapInputActionNames.end())
	{
		std::map<int, CInputActionNameRow*>::iterator iter = m_mapInputActions.find(itName->second);
		if (iter != m_mapInputActions.end())
			return iter->second;
	}
	return NULL;
}

const std::map<int, CInputActionNameRow*>& COtherTableCenter::GetInputActionNameMap()
{
	return m_mapInputActions;
}

CInputKeyRow* COtherTableCenter::GetInputKeyRow(int iID)
{
	std::map<int, CInputKeyRow*>::iterator iter = m_mapInputKeys.find(iID);
	if (iter != m_mapInputKeys.end())
		return iter->second;
	return NULL;
}

CInputKeyRow* COtherTableCenter::GetInputKeyRowByName(const std::string& strKeyName)
{
	std::map<std::string, int>::iterator itName = m_mapInputKeyNames.find(strKeyName);
	if (itName != m_mapInputKeyNames.end())
	{
		std::map<int, CInputKeyRow*>::iterator iter = m_mapInputKeys.find(itName->second);
		if (iter != m_mapInputKeys.end())
			return iter->second;
	}
	return NULL;
}