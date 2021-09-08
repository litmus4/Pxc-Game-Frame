// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PxcNetGameMode.h"
#include "PxcGameState.h"
#include "NetBlueprintLibrary.h"

void APxcNetGameMode::MulticastUniversal(FName TypeName, UObject* pParamObject, FSharedSignature ParamSig)
{
	if (!ensure(GetNetMode() == NM_ListenServer))
		return;

	APxcGameState* pGS = GetGameState<APxcGameState>();
	if (pGS)
		pGS->MulticastUniversal(TypeName, pParamObject, ParamSig, false);
}

void APxcNetGameMode::MulticastUniversalSync(FName TypeName, float fWaitTime, FSimpleDynamicDelexPair DelexCompleted,
	UObject* pParamObject, FSharedSignature ParamSig)
{
	if (!ensure(GetNetMode() == NM_ListenServer) ||
		m_tmapServerSyncInfos.Contains(EServerSyncType::Universal))
		return;

	APxcGameState* pGS = GetGameState<APxcGameState>();
	if (pGS)
		pGS->MulticastUniversal(TypeName, pParamObject, ParamSig, true);

	AddWaitingUsedServerSyncInfo(EServerSyncType::Universal, fWaitTime, 0, DelexCompleted);
}

FServerSyncInfo* APxcNetGameMode::FindServerSyncInfo(EServerSyncType eType)
{
	return m_tmapServerSyncInfos.Find(eType);
}

bool APxcNetGameMode::IsFlowFenceServerReady(const FName& CheckName)
{
	int32* pNum = m_tmapFlowFenceSyncCache.Find(CheckName);
	if (pNum)
		return (*pNum < 0);
	return false;
}

void APxcNetGameMode::ReadyFlowFence(const FName& CheckName)
{
	FServerSyncInfo* pInfo = m_tmapServerSyncInfos.Find(EServerSyncType::FlowFence);
	if (pInfo)
	{
		if (IsFlowFenceServerReady(CheckName))
			pInfo->UpdateComplete(this);
	}
	else
	{
		int32* pNum = m_tmapFlowFenceSyncCache.Find(CheckName);
		if (!pNum)
			m_tmapFlowFenceSyncCache.Add(CheckName, 1);
		else if (*pNum >= 0)
			(*pNum)++;
	}
}

void APxcNetGameMode::ReadyFlowFenceOnServer(const FName& CheckName, float fWaitTime, FSimpleDynamicDelexPair& DelexCompleted)
{
	if (!ensure(GetNetMode() == NM_ListenServer) ||
		m_tmapServerSyncInfos.Contains(EServerSyncType::FlowFence))
		return;

	int32* pNum = m_tmapFlowFenceSyncCache.Find(CheckName);
	if (pNum)
	{
		if (ensureAlways(*pNum >= 0))
		{
			(*pNum)++;
			if (*pNum < UNetBlueprintLibrary::Net_GetPlayerCount(this))
			{
				AddWaitingUsedServerSyncInfo(EServerSyncType::FlowFence, fWaitTime, *pNum, DelexCompleted);
				*pNum = -1;
			}
			else
				DelexCompleted.DeleSucceeded.ExecuteIfBound();
		}
	}
	else
	{
		AddWaitingUsedServerSyncInfo(EServerSyncType::FlowFence, fWaitTime, 1, DelexCompleted);
		m_tmapFlowFenceSyncCache.Add(CheckName, -1);
	}
}

void APxcNetGameMode::RemoveFlowFence(const FName& CheckName)
{
	m_tmapFlowFenceSyncCache.Remove(CheckName);
}

void APxcNetGameMode::AddWaitingUsedServerSyncInfo(EServerSyncType eType, float fWaitTime, int32 iAlreadyNum,
	FSimpleDynamicDelexPair& DelexCompleted)
{
	FServerSyncInfo Info(eType);
	if (iAlreadyNum > 0) Info.iCompletedNum = iAlreadyNum;

	Info.Delegate.BindLambda([this, eType, DelexCompleted]() {
		DelexCompleted.DeleSucceeded.ExecuteIfBound();
		RemoveServerSyncInfo(eType);
	});

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, eType, DelexCompleted]() {
		DelexCompleted.DeleFailed.ExecuteIfBound();
		m_tmapServerSyncInfos.Remove(eType);
	});
	GetWorldTimerManager().SetTimer(Info.Timer, TimerDelegate, fWaitTime, false);

	m_tmapServerSyncInfos.Add(eType, Info);
}

void APxcNetGameMode::AddWaitingUsedServerSyncInfo(EServerSyncType eType, float fWaitTime, int32 iAlreadyNum,
	FSimpleDelegate&& DeleSucceeded, FSimpleDelegate&& DeleFailed)
{
	FServerSyncInfo Info(eType);
	if (iAlreadyNum > 0) Info.iCompletedNum = iAlreadyNum;

	Info.Delegate.BindLambda([this, eType, DeleSucceeded]() {
		DeleSucceeded.ExecuteIfBound();
		RemoveServerSyncInfo(eType);
	});

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this, eType, DeleFailed]() {
		DeleFailed.ExecuteIfBound();
		m_tmapServerSyncInfos.Remove(eType);
	});
	GetWorldTimerManager().SetTimer(Info.Timer, TimerDelegate, fWaitTime, false);

	m_tmapServerSyncInfos.Add(eType, Info);
}

void APxcNetGameMode::RemoveServerSyncInfo(EServerSyncType eType)
{
	FServerSyncInfo* pInfo = m_tmapServerSyncInfos.Find(eType);
	if (pInfo)
	{
		if (GetWorldTimerManager().IsTimerActive(pInfo->Timer))
			GetWorldTimerManager().ClearTimer(pInfo->Timer);
		m_tmapServerSyncInfos.Remove(eType);
	}
}