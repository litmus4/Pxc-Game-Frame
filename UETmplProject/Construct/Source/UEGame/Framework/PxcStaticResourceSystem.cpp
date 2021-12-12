// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcStaticResourceSystem.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"
#include "Framework/PxcGameConfig.h"
#include "Framework/PxcBlueprintLibrary.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/TextTable/TextTableCenter.h"
#include "DataTables/StoryTable/StoryTableCenter.h"
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "PxcUtil/StringTools.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "PxcUtil/zPackEx.h"

UPxcStaticResourceSystem* UPxcStaticResourceSystem::s_pInst = nullptr;

UPxcStaticResourceSystem* UPxcStaticResourceSystem::GetInstance()
{
	check(s_pInst);
	return s_pInst;
}

void UPxcStaticResourceSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	s_pInst = this;
	std::string strContentDir = TCHAR_TO_ANSI(*FPaths::ProjectContentDir());

	//DataTables
	CTextTableCenter::GetInstance()->Init(strContentDir, GlobalDef::ELanguage_ZH_CN);
	CStoryTableCenter::GetInstance()->Init(strContentDir);
	COtherTableCenter::GetInstance()->Init(strContentDir);

	PxcUtil::zPackRelease();
	const UPxcGameConfig* pConfig = GetDefault<UPxcGameConfig>();
	check(pConfig);

	//UnrealDataTables
	UDataTable* pUDT = pConfig->GEExtentionContToInstUDT.LoadSynchronous();
	if (pUDT)
	{
		LOAD_UDT_TO_TMAP(pUDT, FUTRGExContToInst, m_tmapUDTGEExtentionContToInst, ExtentionTag);
	}

	UE_LOG(LogTemp, Log, TEXT("@@@@@ PxcStaticResourceSystem initialized"));
}

void UPxcStaticResourceSystem::Deinitialize()
{
	CTextTableCenter::GetInstance()->Release();
	CTextTableCenter::DeleteInstance();
	CStoryTableCenter::GetInstance()->Release();
	CStoryTableCenter::DeleteInstance();
	COtherTableCenter::GetInstance()->Release();
	COtherTableCenter::DeleteInstance();

	PxcUtil::zPackRelease();
	UE_LOG(LogTemp, Log, TEXT("@@@@@ PxcStaticResourceSystem deinitialized"));
}

const FUTRGExContToInst* UPxcStaticResourceSystem::GetUTRGEExtentionContToInst(const FGameplayTag& ExtentionTag)
{
	const FUTRGExContToInst* pRow = m_tmapUDTGEExtentionContToInst.Find(ExtentionTag);
	return pRow;
}