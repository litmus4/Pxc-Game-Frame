// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcGameMode.h"
#include "LevelMgrs/VirtualGroupMgr.h"
#include "LevelMgrs/RelativeTimeDilationMgr.h"

//*≤‚ ‘¡Ÿ ±
#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/TextTable/TextTableCenter.h"
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "PxcUtil/StringTools.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"
#include <string>
//*/

APxcGameMode::APxcGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	m_pVirtualGroupMgr = CreateDefaultSubobject<UVirtualGroupMgr>(TEXT("VirtualGroupMgr"));
	m_pRelTimeDilationMgr = CreateDefaultSubobject<URelativeTimeDilationMgr>(TEXT("RelativeTimeDilationMgr"));
}

void APxcGameMode::InitGame(const FString& sMapName, const FString& sOptions, FString& sErrorMessage)
{
	Super::InitGame(sMapName, sOptions, sErrorMessage);

	//*≤‚ ‘¡Ÿ ±
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
	FString sText5 = PxcUtil::StringTools::Utf8ToWstr(strText5).c_str();
	UE_LOG(LogTemp, Log, TEXT("@@@@@@@@@@@@@@@@@ %s"), *sText5);
	OnHelloWorld(sText5);
	//*/
}

void APxcGameMode::Tick(float fDeltaSeconds)
{
	Super::Tick(fDeltaSeconds);

	m_pRelTimeDilationMgr->Tick(fDeltaSeconds);
}

void APxcGameMode::EndPlay(const EEndPlayReason::Type eEndPlayReason)
{
	m_pRelTimeDilationMgr->Release();
	m_pVirtualGroupMgr->Release();

	Super::EndPlay(eEndPlayReason);
}

UVirtualGroupMgr* APxcGameMode::GetVirtualGroupMgr()
{
	return m_pVirtualGroupMgr;
}

URelativeTimeDilationMgr* APxcGameMode::GetRelativeTimeDilationMgr()
{
	return m_pRelTimeDilationMgr;
}

UGroupCentralTargetMgr* APxcGameMode::GetGroupCentralTargetMgr()
{
	return m_pGroupCentralTargetMgr;
}