// Fill out your copyright notice in the Description page of Project Settings.


#include "PXCycleInstance.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"
#include "GameFramework/GameModeBase.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "PxcUtil/zPackEx.h"
//#include <string>

#define USE_ZPACK 2

void UPXCycleInstance::Init()
{
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
	UE_LOG(LogTemp, Log, TEXT("@@@@@ PXCycleInstance top init end"));

	Super::Init();

	FGameModeEvents::GameModeInitializedEvent.AddUObject(this, &UPXCycleInstance::OnGameModeInitialized);
}

void UPXCycleInstance::OnGameModeInitialized(AGameModeBase* pGM)
{
	//
}