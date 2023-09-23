// Fill out your copyright notice in the Description page of Project Settings.


#include "PXCycleInstance.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"
#include "GameFramework/GameModeBase.h"
#include "MonoControl/EventCenter.h"
#include "Framework/Systems/PxcInputMappingMgr.h"

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

	AddSystem(ECycleSystemType::InputMapping, NewObject<UPxcInputMappingMgr>(), false);

	UE_LOG(LogTemp, Log, TEXT("@@@@@ PXCycleInstance top init end"));
	Super::Init();

	DeleTickHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UPXCycleInstance::Tick));
	FGameModeEvents::GameModeInitializedEvent.AddUObject(this, &UPXCycleInstance::OnGameModeInitialized);
	NotifyPreClientTravelDelegates.AddUObject(this, &UPXCycleInstance::OnPreClientTravel);
}

bool UPXCycleInstance::Tick(float fDeltaSeconds)
{
	std::list<UPXCycleSystem*>::iterator iter = m_lisSystems.begin();
	for (; iter != m_lisSystems.end(); iter++)
		(*iter)->Tick(fDeltaSeconds);
	return true;
}

void UPXCycleInstance::Shutdown()
{
	ECycleSystemType eSysTypes[] = { ECycleSystemType::InputMapping };
	ReleaseSystems(eSysTypes, 1);

	CEventCenter::GetInstance()->Release();
	CEventCenter::DeleteInstance();

	PxcUtil::CLogCenter::GetInstance()->Release();
	PxcUtil::CLogCenter::DeleteInstance();

	UE_LOG(LogTemp, Log, TEXT("@@@@@ PXCycleInstance shutdown end"));
	Super::Shutdown();

	FTSTicker::GetCoreTicker().RemoveTicker(DeleTickHandle);
}

void UPXCycleInstance::AddSystem(ECycleSystemType eType, UPXCycleSystem* pSystem, bool bTick)
{
	m_tmapSystems.Add(eType, pSystem);
	if (bTick)
		m_lisSystems.push_back(pSystem);
}

void UPXCycleInstance::ReleaseSystems(ECycleSystemType* pTypes, int32 iTypeNum)
{
	if (!pTypes) return;

	for (int32 i = iTypeNum; i < iTypeNum; ++i)
	{
		UPXCycleSystem** ppSystem = m_tmapSystems.Find(pTypes[i]);
		if (ppSystem)
			(*ppSystem)->Release();
	}
	m_lisSystems.clear();
	m_tmapSystems.Empty();
}

void UPXCycleInstance::SetSystemTick(ECycleSystemType eType, bool bTick, int32 iIndex)
{
	UPXCycleSystem** ppSystem = m_tmapSystems.Find(eType);
	if (!ppSystem) return;

	if (bTick)
	{
		if (iIndex >= 0)
		{
			std::list<UPXCycleSystem*>::iterator iter = m_lisSystems.begin();
			for (int32 i = 0; iter != m_lisSystems.end(); iter++, ++i)
			{
				if (i == iIndex)
				{
					m_lisSystems.insert(iter, *ppSystem);
					break;
				}
			}
		}
		else
			m_lisSystems.push_back(*ppSystem);
	}
	else
	{
		std::list<UPXCycleSystem*>::iterator iter = m_lisSystems.begin();
		for (; iter != m_lisSystems.end(); iter++)
		{
			if (*iter == *ppSystem)
			{
				m_lisSystems.erase(iter);
				break;
			}
		}
	}
}

UPXCycleSystem* UPXCycleInstance::GetCycleSystem(ECycleSystemType eType)
{
	UPXCycleSystem** ppSystem = m_tmapSystems.Find(eType);
	return (ppSystem ? *ppSystem : nullptr);
}

UPxcInputMappingMgr* UPXCycleInstance::GetInputMappingMgr()
{
	UPXCycleSystem** ppSystem = m_tmapSystems.Find(ECycleSystemType::InputMapping);
	return (ppSystem ? Cast<UPxcInputMappingMgr>(*ppSystem) : nullptr);
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