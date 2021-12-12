// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/FlowFenceActorWithKey.h"
#include "PxcGameState.h"
#include "Framework/PxcAssistantSubsystem.h"
#include "PxcNetGameMode.h"
#include "NetBlueprintLibrary.h"

UFlowFenceActorWithKey::UFlowFenceActorWithKey(const FObjectInitializer& ObjectInitializer)
{
	//
}

UFlowFenceActorWithKey* UFlowFenceActorWithKey::StartFlowFence(UObject* pWCO, FName CheckName, float fWaitTime)
{
	UWorld* pWorld = GEngine->GetWorldFromContextObject(pWCO, EGetWorldErrorMode::LogAndReturnNull);
	UFlowFenceActorWithKey* pFence = NewObject<UFlowFenceActorWithKey>(pWorld);
	if (pFence)
	{
		pFence->m_CheckName = CheckName;
		pFence->m_fWaitTime = fWaitTime;
	}
	return pFence;
}

void UFlowFenceActorWithKey::Activate()
{
	Super::Activate();

	AGameStateBase* pGS = GetWorld()->GetGameState();
	ENetMode eNetMode = GetWorld()->GetNetMode();
	if (!pGS || eNetMode == NM_Standalone)
	{
		OnEnded(true);
		return;
	}

	UPxcAssistantSubsystem* pSystem = UPxcAssistantSubsystem::GetInstance();
	FString&& sKey = TEXT("FlowFence_") + m_CheckName.ToString();
	if (!pSystem->AddWaitingActorWithKey(pGS, sKey, this))
	{
		OnEnded(true);
		return;
	}

	if (eNetMode == NM_ListenServer)
	{
		APxcNetGameMode* pGM = GetWorld()->GetAuthGameMode<APxcNetGameMode>();
		check(pGM);
		FSimpleDynamicDelexPair Delex;
		Delex.DeleSucceeded.BindDynamic(this, &UFlowFenceActorWithKey::OnCompleteAndFinished);
		Delex.DeleFailed.BindDynamic(this, &UFlowFenceActorWithKey::OnCompleteAndTimeOut);
		pGM->ReadyFlowFenceOnServer(m_CheckName, m_fWaitTime, Delex);
	}
	else
	{
		//TODOJK Player¿‡Õ¯¬Á∞Ê
		//APxcCharacterPlayer* pPlayer = CastChecked<APxcCharacterPlayer>(UNetBlueprintLibrary::Net_GetLocalPlayerPawn(pGS));
		//pPlayer->ServerReadyFlowFence(m_CheckName);

		FTimerDelegate TimerDelegate;
		TimerDelegate.BindUObject(this, &UFlowFenceActorWithKey::OnCompleteAndTimeOut);
		GetWorld()->GetTimerManager().SetTimer(m_ClientTimer, TimerDelegate, m_fWaitTime, false);
	}
}

void UFlowFenceActorWithKey::OnCompleteAndFinished()
{
	APxcGameState* pGS = CastChecked<APxcGameState>(GetWorld()->GetGameState());
	FString&& sKey = TEXT("FlowFence_") + m_CheckName.ToString();

	if (GetWorld()->GetNetMode() == NM_ListenServer)
	{
		pGS->MulticastFinishFlowFence(sKey);

		APxcNetGameMode* pGM = GetWorld()->GetAuthGameMode<APxcNetGameMode>();
		check(pGM);
		pGM->RemoveFlowFence(m_CheckName);
	}
	else if (GetWorld()->GetTimerManager().IsTimerActive(m_ClientTimer))
		GetWorld()->GetTimerManager().ClearTimer(m_ClientTimer);

	UPxcAssistantSubsystem* pSystem = UPxcAssistantSubsystem::GetInstance();
	pSystem->EndWaitingActorWithKey<UFlowFenceActorWithKey>(pGS, sKey, true);
}

void UFlowFenceActorWithKey::OnCompleteAndTimeOut()
{
	if (GetWorld()->GetNetMode() == NM_ListenServer)
	{
		APxcNetGameMode* pGM = GetWorld()->GetAuthGameMode<APxcNetGameMode>();
		check(pGM);
		pGM->RemoveFlowFence(m_CheckName);
	}

	UPxcAssistantSubsystem* pSystem = UPxcAssistantSubsystem::GetInstance();
	AGameStateBase* pGS = GetWorld()->GetGameState();
	check(pGS);
	FString&& sKey = TEXT("FlowFence_") + m_CheckName.ToString();
	pSystem->EndWaitingActorWithKey<UFlowFenceActorWithKey>(pGS, sKey, false);
}

void UFlowFenceActorWithKey::OnEnded(bool bFinish)
{
	if (bFinish)
		DeleFinished.Broadcast();
	else
		DeleTimeOut.Broadcast();
}