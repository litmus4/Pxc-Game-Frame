// Fill out your copyright notice in the Description page of Project Settings.


#include "AsyncWaitActorWithKey.h"
#include "Framework/PxcAssistantSubsystem.h"

UAsyncWaitActorWithKey::UAsyncWaitActorWithKey(const FObjectInitializer& ObjectInitializer)
{
	//
}

UAsyncWaitActorWithKey* UAsyncWaitActorWithKey::AsyncUtil_WaitActorWithKey(AActor* pActor, const FString& sKey, float fWaitTime)
{
	if (!pActor) return nullptr;

	UAsyncWaitActorWithKey* pWait = NewObject<UAsyncWaitActorWithKey>();
	if (pWait)
	{
		pWait->m_pActor = pActor;
		pWait->m_sKey = sKey;
		pWait->m_fWaitTime = fWaitTime;
	}
	return pWait;
}

void UAsyncWaitActorWithKey::Activate()
{
	Super::Activate();
	check(m_pActor);

	UPxcAssistantSubsystem* pSystem = UPxcAssistantSubsystem::GetInstance();
	if (!pSystem->AddWaitingActorWithKey(m_pActor, m_sKey, this))
	{
		OnEnded(true);
		return;
	}

	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([pSystem, this]() {
		pSystem->EndWaitingActorWithKey<UAsyncWaitActorWithKey>(m_pActor, m_sKey, false);
	});
	m_pActor->GetWorldTimerManager().SetTimer(m_Timer, TimerDelegate, m_fWaitTime, false);
}

void UAsyncWaitActorWithKey::OnEnded(bool bFinish)
{
	check(m_pActor);

	if (bFinish)
	{
		if (m_pActor->GetWorldTimerManager().IsTimerActive(m_Timer))
			m_pActor->GetWorldTimerManager().ClearTimer(m_Timer);

		DeleFinished.Broadcast();
	}
	else
		DeleTimeOut.Broadcast();
}
