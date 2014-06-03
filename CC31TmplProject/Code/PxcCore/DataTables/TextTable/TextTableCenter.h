#pragma once
#include "PxcUtil/Singleton.h"
#include "TextRow.h"
#include <map>

class CTextTableCenter : public PxcUtil::Singleton<CTextTableCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(CTextTableCenter)
public:
	~CTextTableCenter();
	bool Init();
	void Release();

	CTextRow* GetTextRow(int iID);
	std::string GetText(int iID);

private:
	std::map<int, CTextRow*> m_mapTexts;
};