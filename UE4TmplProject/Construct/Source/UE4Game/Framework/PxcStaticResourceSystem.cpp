// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcStaticResourceSystem.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/TextTable/TextTableCenter.h"
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "PxcUtil/StringTools.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include "PxcUtil/zPackEx.h"
//#include <string>

void UPxcStaticResourceSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	std::string strContentDir = TCHAR_TO_ANSI(*FPaths::ProjectContentDir());

	//DataTables
	CTextTableCenter::GetInstance()->Init(strContentDir, GlobalDef::ELanguage_ZH_CN);
	COtherTableCenter::GetInstance()->Init(strContentDir);

	PxcUtil::zPackRelease();

	UE_LOG(LogTemp, Log, TEXT("@@@@@ PxcStaticResourceSystem initialized"));
}

void UPxcStaticResourceSystem::Deinitialize()
{
	CTextTableCenter::GetInstance()->Release();
	COtherTableCenter::GetInstance()->Release();

	PxcUtil::zPackRelease();
	UE_LOG(LogTemp, Log, TEXT("@@@@@ PxcStaticResourceSystem deinitialized"));
}