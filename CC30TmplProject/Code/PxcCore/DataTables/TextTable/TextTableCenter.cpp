#include "TextTableCenter.h"
#include "..\LoadDefines.h"

CTextTableCenter::CTextTableCenter()
{
	//
}

CTextTableCenter::~CTextTableCenter()
{
	//
}

bool CTextTableCenter::Init()
{
	LOADTABLE(Text, "TextTable", m_mapTexts, m_iID)
	return true;
}

void CTextTableCenter::Release()
{
	UNLOADTABLE(Text, m_mapTexts)
}

CTextRow* CTextTableCenter::GetTextRow(int iID)
{
	std::map<int, CTextRow*>::iterator iter = m_mapTexts.find(iID);
	if (iter != m_mapTexts.end())
		return iter->second;
	return NULL;
}

std::string CTextTableCenter::GetText(int iID)
{
	std::map<int, CTextRow*>::iterator iter = m_mapTexts.find(iID);
	if (iter != m_mapTexts.end())
		return iter->second->m_strText;
	return "";
}