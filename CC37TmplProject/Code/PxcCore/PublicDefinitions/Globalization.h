#pragma once

namespace GlobalDef
{

enum ELanguage
{
	ELanguage_ZH_CN = 0,
	ELanguage_EN_US,
	ELanguage_ZH_TW,
};

const char* g_szLanguageFolders[] = {
	"\\zh_cn",
	"\\en_us",
	"\\zh_tw",
};

}