#pragma once
#include "PxcUtil/Singleton.h"
#include "GlobalParamRow.h"
#include "InputKeyRow.h"

class COtherTableCenter : public PxcUtil::Singleton<COtherTableCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(COtherTableCenter)
public:
	~COtherTableCenter();
	bool Init(const std::string& strPath);
	void Release();

	CGlobalParamRow* GetGlobalParamRow(int iID);

	CInputKeyRow* GetInputKeyRow(int iID);
	CInputKeyRow* GetInputKeyRowByName(const std::string& strKeyName);

private:
	std::map<int, CGlobalParamRow*> m_mapGlobalParams;

	std::map<int, CInputKeyRow*> m_mapInputKeys;
	std::map<std::string, int> m_mapInputKeyNames;
};