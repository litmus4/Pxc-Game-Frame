#pragma once
#include "PxcUtil/Singleton.h"
#include "GlobalParamRow.h"

class COtherTableCenter : public PxcUtil::Singleton<COtherTableCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(COtherTableCenter)
public:
	~COtherTableCenter();
	bool Init(const std::string& strPath);
	void Release();

	CGlobalParamRow* GetGlobalParamRow(int iID);

private:
	std::map<int, CGlobalParamRow*> m_mapGlobalParams;
};