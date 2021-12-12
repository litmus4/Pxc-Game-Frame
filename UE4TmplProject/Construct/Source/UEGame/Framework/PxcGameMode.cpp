// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcGameMode.h"
#include "LevelMgrs/VirtualGroupMgr.h"
#include "LevelMgrs/RelativeTimeDilationMgr.h"
#include "LevelMgrs/GroupCentralTargetMgr.h"

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
}

void APxcGameMode::InitGame(const FString& sMapName, const FString& sOptions, FString& sErrorMessage)
{
	Super::InitGame(sMapName, sOptions, sErrorMessage);

	AddSpecialLevelManager(ESpecialLevelMgrType::RelativeTimeDilation, NewObject<URelativeTimeDilationMgr>(this));
	AddSpecialLevelManager(ESpecialLevelMgrType::CentralTarget, NewObject<UGroupCentralTargetMgr>(this));

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

	std::list<USpecialLevelMgrBase*>::iterator iter = m_lisSpecialMgrs.begin();
	for (; iter != m_lisSpecialMgrs.end(); iter++)
		(*iter)->Tick(fDeltaSeconds);
}

void APxcGameMode::EndPlay(const EEndPlayReason::Type eEndPlayReason)
{
	ESpecialLevelMgrType eMgrTypes[] = { ESpecialLevelMgrType::RelativeTimeDilation,
									ESpecialLevelMgrType::CentralTarget };
	ReleaseSpecialLevelManagers(eMgrTypes, 2);
	m_pVirtualGroupMgr->Release();

	Super::EndPlay(eEndPlayReason);
}

void APxcGameMode::AddSpecialLevelManager(ESpecialLevelMgrType eType, USpecialLevelMgrBase* pMgr, bool bTick)
{
	m_tmapSpecialMgrs.Add(eType, pMgr);
	if (bTick)
		m_lisSpecialMgrs.push_back(pMgr);
}

void APxcGameMode::ReleaseSpecialLevelManagers(ESpecialLevelMgrType* pTypes, int32 iTypeNum)
{
	if (!pTypes) return;

	for (int32 i = iTypeNum; i < iTypeNum; ++i)
	{
		USpecialLevelMgrBase** ppMgr = m_tmapSpecialMgrs.Find(pTypes[i]);
		if (ppMgr)
			(*ppMgr)->Release();
	}
	m_lisSpecialMgrs.clear();
	m_tmapSpecialMgrs.Empty();
}

void APxcGameMode::SetSpecialLevelManagerTick(ESpecialLevelMgrType eType, bool bTick, int32 iIndex)
{
	USpecialLevelMgrBase** ppMgr = m_tmapSpecialMgrs.Find(eType);
	if (!ppMgr) return;

	if (bTick)
	{
		if (iIndex >= 0)
		{
			std::list<USpecialLevelMgrBase*>::iterator iter = m_lisSpecialMgrs.begin();
			for (int32 i = 0; iter != m_lisSpecialMgrs.end(); iter++, ++i)
			{
				if (i == iIndex)
				{
					m_lisSpecialMgrs.insert(iter, *ppMgr);
					break;
				}
			}
		}
		else
			m_lisSpecialMgrs.push_back(*ppMgr);
	}
	else
	{
		std::list<USpecialLevelMgrBase*>::iterator iter = m_lisSpecialMgrs.begin();
		for (; iter != m_lisSpecialMgrs.end(); iter++)
		{
			if (*iter == *ppMgr)
			{
				m_lisSpecialMgrs.erase(iter);
				break;
			}
		}
	}
}

UVirtualGroupMgr* APxcGameMode::GetVirtualGroupMgr()
{
	return m_pVirtualGroupMgr;
}

USpecialLevelMgrBase* APxcGameMode::GetSpecialLevelManager(ESpecialLevelMgrType eType)
{
	USpecialLevelMgrBase** ppMgr = m_tmapSpecialMgrs.Find(eType);
	return (ppMgr ? *ppMgr : nullptr);
}

URelativeTimeDilationMgr* APxcGameMode::GetRelativeTimeDilationMgr()
{
	USpecialLevelMgrBase** ppMgr = m_tmapSpecialMgrs.Find(ESpecialLevelMgrType::RelativeTimeDilation);
	return (ppMgr ? Cast<URelativeTimeDilationMgr>(*ppMgr) : nullptr);
}

UGroupCentralTargetMgr* APxcGameMode::GetGroupCentralTargetMgr()
{
	USpecialLevelMgrBase** ppMgr = m_tmapSpecialMgrs.Find(ESpecialLevelMgrType::CentralTarget);
	return (ppMgr ? Cast<UGroupCentralTargetMgr>(*ppMgr) : nullptr);
}