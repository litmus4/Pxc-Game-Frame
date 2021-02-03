#pragma once
#include "PxcUtil/Singleton.h"
#include "GlobalParamRow.h"
#include "InputActionNameRow.h"
#include "InputKeyRow.h"

class COtherTableCenter : public PxcUtil::Singleton<COtherTableCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(COtherTableCenter)
public:
	~COtherTableCenter();
	bool Init(const std::string& strPath);
	void Release();

	CGlobalParamRow* GetGlobalParamRow(int iID);

	static std::string LinkAxisName(const std::string& strAxisName, bool bPositiveDir);
	bool LoadInputActionNames();
	CInputActionNameRow* GetInputActionNameRow(int iID);
	CInputActionNameRow* GetInputActionNameRowByName(const std::string& strActionName);
	const std::map<int, CInputActionNameRow*>& GetInputActionNameMap();

	CInputKeyRow* GetInputKeyRow(int iID);
	CInputKeyRow* GetInputKeyRowByName(const std::string& strKeyName);

private:
	std::map<int, CGlobalParamRow*> m_mapGlobalParams;

	std::map<int, CInputActionNameRow*> m_mapInputActions;
	std::map<std::string, int> m_mapInputActionNames;

	std::map<int, CInputKeyRow*> m_mapInputKeys;
	std::map<std::string, int> m_mapInputKeyNames;

	std::string m_strPath;
};