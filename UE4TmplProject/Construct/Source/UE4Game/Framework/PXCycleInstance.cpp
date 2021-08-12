// Fill out your copyright notice in the Description page of Project Settings.


#include "PXCycleInstance.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"
#include "GameFramework/GameModeBase.h"
#include "MonoControl/EventCenter.h"
#include "Framework/PxcInputMappingMgr.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "PxcUtil/zPackEx.h"

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
	PxcUtil::zPackAddPathAim("Packs\\DataTables.zpk", "DataTables");
	//PxcUtil::zPackAddPathAim("Packs\\Assets.zpk", "Assets");
#else
	//TODOJK 暂时只有Win64/32平台
#endif

	m_pInputMappingMgr = NewObject<UPxcInputMappingMgr>();

	UE_LOG(LogTemp, Log, TEXT("@@@@@ PXCycleInstance top init end"));
	Super::Init();

	FGameModeEvents::GameModeInitializedEvent.AddUObject(this, &UPXCycleInstance::OnGameModeInitialized);
	NotifyPreClientTravelDelegates.AddUObject(this, &UPXCycleInstance::OnPreClientTravel);
}

void UPXCycleInstance::Shutdown()
{
	CEventCenter::GetInstance()->Release();
	CEventCenter::DeleteInstance();

	PxcUtil::CLogCenter::GetInstance()->Release();
	PxcUtil::CLogCenter::DeleteInstance();

	UE_LOG(LogTemp, Log, TEXT("@@@@@ PXCycleInstance shutdown end"));
	Super::Shutdown();
}

UPxcInputMappingMgr* UPXCycleInstance::GetInputMappingMgr()
{
	return m_pInputMappingMgr;
}

void UPXCycleInstance::OnGameModeInitialized(AGameModeBase* pGM)
{
	//
}

void UPXCycleInstance::OnPreClientTravel(const FString& sPendingURL, ETravelType eTravelType, bool bIsSeamlessTravel)
{
	if (!GetWorld() || GetWorld()->GetNetMode() != NM_Client)
		return;
	//
}