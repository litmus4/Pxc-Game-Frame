#pragma once
#include <string>

namespace GlobalDef
{

enum ELanguage
{
	ELanguage_ZH_CN = 0,
	ELanguage_EN_US,
	ELanguage_ZH_TW,
};

inline const char* GetLanguageFolder(ELanguage eLanguage)
{
	static const char* szLanguageFolders[] = {
		"\\zh_cn",
		"\\en_us",
		"\\zh_tw",
	};
	return szLanguageFolders[eLanguage];
}

inline bool InsertLanguageFolder(std::string& strPath, ELanguage eLanguage)
{
	int ipos = strPath.find_last_of('\\');
	if (ipos != std::string::npos)
	{
		std::string strFolder = strPath.substr(0, ipos);
		std::string strFile = strPath.substr(ipos);
		strPath = strFolder + GetLanguageFolder(eLanguage) + strFile;
		return true;
	}
	return false;
}

}