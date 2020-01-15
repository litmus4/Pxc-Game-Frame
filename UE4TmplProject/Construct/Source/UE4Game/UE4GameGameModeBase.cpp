// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.


#include "UE4GameGameModeBase.h"
#include "Misc/Paths.h"
#include "Containers/StringConv.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/LogCenter.h"
#include "PublicDefinitions/SpecialFileDef.h"
//#include "DataTables/TextTable/TextTableCenter.h"
//#include "DataTables/OtherTable/OtherTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
//#include "PxcUtil/zPackEx.h"
#include <string>

#define USE_ZPACK 2

AUE4GameGameModeBase::AUE4GameGameModeBase()
{
	//
}

void AUE4GameGameModeBase::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	AGameModeBase::InitGame(MapName, Options, ErrorMessage);

	std::string strDir = TCHAR_TO_ANSI(*FPaths::ProjectSavedDir());
	PXCU_LOGINST->Init(SpecialFileDef::ELogFile_AssetsTables, (strDir + "Logs\\log_assetstables.txt").c_str());
	//
}