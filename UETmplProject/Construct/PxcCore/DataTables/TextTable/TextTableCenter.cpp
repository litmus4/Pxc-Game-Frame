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

bool CTextTableCenter::Init(const std::string& strPath, GlobalDef::ELanguage eLanguage, bool bUseCaptionTags)
{
	std::string strFolder = "TextTable";
	strFolder += GlobalDef::GetLanguageFolder(eLanguage);

	LOADTABLE(Text, strPath, strFolder, m_mapTexts, m_iID)
	LOADTABLE(Caption, strPath, strFolder, m_mapCaptions, m_iID)
	if (bUseCaptionTags)
	{
		std::map<int, CCaptionRow*>::iterator iter = m_mapCaptions.begin();
		for (; iter != m_mapCaptions.end(); iter++)
			m_mapCaptionTags.insert(std::make_pair(iter->second->m_strTag, iter->first));
	}
	LOADTABLE(StoryText, strPath, strFolder, m_mapStoryTexts, m_iID)
	return true;
}

void CTextTableCenter::Release()
{
	UNLOADTABLE(Text, m_mapTexts)
	UNLOADTABLE(Caption, m_mapCaptions)
	m_mapCaptionTags.clear();
	UNLOADTABLE(StoryText, m_mapStoryTexts)
	DeleteInstance();
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

CCaptionRow* CTextTableCenter::GetCaptionRow(int iID)
{
	std::map<int, CCaptionRow*>::iterator iter = m_mapCaptions.find(iID);
	if (iter != m_mapCaptions.end())
		return iter->second;
	return NULL;
}

std::string CTextTableCenter::GetCaption(int iID)
{
	std::map<int, CCaptionRow*>::iterator iter = m_mapCaptions.find(iID);
	if (iter != m_mapCaptions.end())
		return iter->second->m_strText;
	return "";
}

std::string CTextTableCenter::GetCaptionByTag(const std::string& strTag)
{
	std::map<std::string, int>::iterator itTag = m_mapCaptionTags.find(strTag);
	if (itTag != m_mapCaptionTags.end())
	{
		std::map<int, CCaptionRow*>::iterator iter = m_mapCaptions.find(itTag->second);
		if (iter != m_mapCaptions.end())
			return iter->second->m_strText;
	}
	return "";
}

CStoryTextRow* CTextTableCenter::GetStoryTextRow(int iID)
{
	std::map<int, CStoryTextRow*>::iterator iter = m_mapStoryTexts.find(iID);
	if (iter != m_mapStoryTexts.end())
		return iter->second;
	return NULL;
}

std::string CTextTableCenter::GetStoryText(int iID)
{
	std::map<int, CStoryTextRow*>::iterator iter = m_mapStoryTexts.find(iID);
	if (iter != m_mapStoryTexts.end())
		return iter->second->m_strText;
	return "";
}