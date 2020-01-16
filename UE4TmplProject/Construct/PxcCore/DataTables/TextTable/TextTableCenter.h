#pragma once
#include "PxcUtil/Singleton.h"
#include "PublicDefinitions/Globalization.h"
#include "TextRow.h"
#include "CaptionRow.h"
#include "StoryTextRow.h"

class CTextTableCenter : public PxcUtil::Singleton<CTextTableCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(CTextTableCenter)
public:
	~CTextTableCenter();
	bool Init(const std::string& strPath, GlobalDef::ELanguage eLanguage, bool bUseCaptionTags = false);
	void Release();

	CTextRow* GetTextRow(int iID);
	std::string GetText(int iID);
	CCaptionRow* GetCaptionRow(int iID);
	std::string GetCaption(int iID);
	std::string GetCaptionByTag(const std::string& strTag);
	CStoryTextRow* GetStoryTextRow(int iID);
	std::string GetStoryText(int iID);

private:
	std::map<int, CTextRow*> m_mapTexts;
	std::map<int, CCaptionRow*> m_mapCaptions;
	std::map<std::string, int> m_mapCaptionTags;
	std::map<int, CStoryTextRow*> m_mapStoryTexts;
};