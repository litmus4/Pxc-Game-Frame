// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PxcNetGameMode.h"

FServerSyncInfo* APxcNetGameMode::FindServerSyncInfo(EServerSyncType eType)
{
	return m_tmapServerSyncInfos.Find(eType);
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