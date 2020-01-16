// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "UE4GameGameModeBase.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
#include "DataTables/TextTable/TextTableCenter.h"//FLAGJK ����������������֮һ��Win64��size_t std::string::nposΪlong long��StringParser��int��ֵ�������
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "PxcUtil/zPackEx.h"//FLAGJK ����������������֮����Win32������Win64ͬ�࣬����Ϊx64
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
	//TODOJK ��ʱֻ��Win64/32ƽ̨
#endif
	CTextTableCenter::GetInstance()->Init(strContentDir, GlobalDef::ELanguage_ZH_CN);
	COtherTableCenter::GetInstance()->Init(strContentDir);
	PxcUtil::zPackRelease();

	//FLAGJK
}