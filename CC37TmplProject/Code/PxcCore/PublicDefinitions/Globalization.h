#pragma once

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

}