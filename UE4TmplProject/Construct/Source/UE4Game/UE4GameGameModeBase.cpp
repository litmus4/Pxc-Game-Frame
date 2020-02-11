// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "UE4GameGameModeBase.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
#include "DataTables/TextTable/TextTableCenter.h"
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "PxcUtil/StringTools.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "PxcUtil/zPackEx.h"
#include <string>

#define USE_ZPACK 1

AUE4GameGameModeBase::AUE4GameGameModeBase()
{
	//
}

void AUE4GameGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameModeBase::InitGame(MapName, Options, ErrorMessage);

	std::string strSavedDir = TCHAR_TO_ANSI(*FPaths::ProjectSavedDir());
	std::string strContentDir = TCHAR_TO_ANSI(*FPaths::ProjectContentDir());
	PXCU_LOGINST->Init(SpecialFileDef::ELogFile_AssetsTables, (strSavedDir + "Logs\\log_assetstables.txt").c_str());
#ifdef USE_ZPACK
#if USE_ZPACK == 2
	PxcUtil::zPackPathSwitch(true, false);
#else
	PxcUtil::zPackPathSwitch(true);
#endif
	PxcUtil::zPackAddPathAim((strContentDir + "Packs\\DataTables.zpk").c_str(), "DataTables");
	//PxcUtil::zPackAddPathAim((strContentDir + "Packs\\Assets.zpk").c_str(), "Assets");
#else
	//TODOJK 暂时只有Win64/32平台
#endif
	CTextTableCenter::GetInstance()->Init(strContentDir, GlobalDef::ELanguage_ZH_CN);
	COtherTableCenter::GetInstance()->Init(strContentDir);
	PxcUtil::zPackRelease();

	std::string strText5 = "Hello World ";
	strText5 += CTextTableCenter::GetInstance()->GetText(5);
	CGlobalParamRow* pParamRow = COtherTableCenter::GetInstance()->GetGlobalParamRow(4);
	if (pParamRow)
		strText5 += PxcUtil::StringTools::BasicToStr(pParamRow->GetLeave<LEAVECOL_I>("Int1"));
	pParamRow = COtherTableCenter::GetInstance()->GetGlobalParamRow(5);
	if (pParamRow)
	{
		std::vector<std::string>& vecValues = pParamRow->GetLeave<LEAVECOL_SA>("Descs");
		if (!vecValues.empty())
			strText5 += vecValues[0];
	}
	FString sText5 = strText5.c_str();
	OnHelloWorld(sText5);
}