// Fill out your copyright notice in the Description page of Project Settings.


#include "PXCycleInstance.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"
#include "GameFramework/GameModeBase.h"
#include "MonoControl/EventCenter.h"
#include "Framework/Systems/PxcInputMappingMgr.h"
#include "Framework/Systems/RandomGameplaySystem.h"
#include "Input/PrlInputMappingContext.h"
#include "PxcNativeLibrary.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "PxcUtil/zPackEx.h"

#define USE_ZPACK 2

UPXCycleInstance* UPXCycleInstance::s_pInst = nullptr;

UPXCycleInstance* UPXCycleInstance::GetInstance()
{
	check(s_pInst);
	return s_pInst;
}

void UPXCycleInstance::Init()
{
	s_pInst = this;

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
	AddSystem(ECycleSystemType::RandomGameplay, NewObject<URandomGameplaySystem>(), false);

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
	ECycleSystemType eSysTypes[] = {
		ECycleSystemType::InputMapping,
		ECycleSystemType::RandomGameplay
	};
	ReleaseSystems(eSysTypes, 2);

	CEventCenter::GetInstance()->Release();
	CEventCenter::DeleteInstance();

	PxcUtil::CLogCenter::GetInstance()->Release();
	PxcUtil::CLogCenter::DeleteInstance();

	ResetDefaultPIMC();

	UE_LOG(LogTemp, Log, TEXT("@@@@@ PXCycleInstance shutdown end"));
	Super::Shutdown();

	FTSTicker::GetCoreTicker().RemoveTicker(DeleTickHandle);
}

void UPXCycleInstance::AddSystem(ECycleSystemType eType, UPXCycleSystem* pSystem, bool bTick)
{
	check(IsValid(pSystem));
	m_tmapSystems.Add(eType, pSystem);
	if (bTick)
		m_lisSystems.push_back(pSystem);
	pSystem->SetCycleGameInstance(this);
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

URandomGameplaySystem* UPXCycleInstance::GetRandomGameplaySystem()
{
	UPXCycleSystem** ppSystem = m_tmapSystems.Find(ECycleSystemType::RandomGameplay);
	return (ppSystem ? Cast<URandomGameplaySystem>(*ppSystem) : nullptr);
}

void UPXCycleInstance::BuildQuickActionMap(bool bRebuild)
{
	if (!m_tmapQuickActions.IsEmpty() && !bRebuild)
		return;
	check(IsValid(m_pPrlIMC));

	m_pPrlIMC->ForEachKeyMappings([this](const FEnhancedActionKeyMapping& KeyMapping) {
		check(KeyMapping.Action);
		FPrlInputActionEx ActionEx;
		ActionEx.pAction = KeyMapping.Action;
		if (ActionEx.pAction->ValueType != EInputActionValueType::Boolean ||
			FPxcNativeLibrary::EI_GetModifierCodeFromTriggers(KeyMapping.Triggers) > 0)
			ActionEx.eMainBindedEvent = ETriggerEvent::Triggered;
		else
			ActionEx.eMainBindedEvent = ETriggerEvent::Started;
		m_tmapQuickActions.Add(ActionEx.pAction->GetFName(), ActionEx);
	});
}

void UPXCycleInstance::SetAndCacheDefaultPIMC(UPrlInputMappingContext* pPrlIMC)
{
	m_pPrlIMC = pPrlIMC;
	if (m_tarrDefaultPIMCached.IsEmpty())
	{
		pPrlIMC->ForEachKeyMappings([this](const FEnhancedActionKeyMapping& KeyMapping) {
			m_tarrDefaultPIMCached.Add(KeyMapping);
		});
	}
}

void UPXCycleInstance::ResetDefaultPIMC()
{
	if (m_bPIMCConfigLoaded && !m_tarrDefaultPIMCached.IsEmpty())
	{
		check(IsValid(m_pPrlIMC));
		m_pPrlIMC->Mappings.Empty();
		for (const FEnhancedActionKeyMapping& KeyMapping : m_tarrDefaultPIMCached)
			m_pPrlIMC->AddMappingQuickly(KeyMapping);
	}
}

const UInputAction* UPXCycleInstance::GetQuickAction(const FName& ActionName) const
{
	const FPrlInputActionEx* pActionEx = m_tmapQuickActions.Find(ActionName);
	return (pActionEx ? pActionEx->pAction : nullptr);
}

ETriggerEvent UPXCycleInstance::GetQuickActionMainBindedEvent(const FName& ActionName) const
{
	const FPrlInputActionEx* pActionEx = m_tmapQuickActions.Find(ActionName);
	return (pActionEx ? pActionEx->eMainBindedEvent : ETriggerEvent::None);
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