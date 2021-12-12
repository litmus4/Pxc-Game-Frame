// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PxcGameState.h"
#include "NetBlueprintLibrary.h"
#include "PrivateDefinitions/NetDef.h"
#include "Framework/PxcAssistantSubsystem.h"
#include "FlowFenceActorWithKey.h"

APxcGameState::APxcGameState()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	//
}

void APxcGameState::OnSetToWorld(APxcGameMode* pGameMode)
{
	K2_OnSetToWorld(pGameMode);
}

void APxcGameState::MulticastUniversal_Implementation(const FName& TypeName,
	UObject* pParamObject, FSharedSignature ParamSig, bool bSync)
{
	OnBroadUniversal(TypeName, pParamObject, ParamSig);

	if (bSync)
	{//TODOJK Player类网络版
		//APxcCharacterPlayer* pPlayer = CastChecked<APxcCharacterPlayer>(UNetBlueprintLibrary::Net_GetLocalPlayerCharacter(this));
		//pPlayer->ServerSyncComplete(EServerSyncType::Universal, true);
	}
}

bool APxcGameState::MulticastUniversal_Validate(const FName& TypeName,
	UObject* pParamObject, FSharedSignature ParamSig, bool bSync)
{
	return true;
}

void APxcGameState::MulticastFinishFlowFence_Implementation(const FString& sKey)
{
	if (GetWorld()->GetNetMode() != NM_Client)//服务器会走自己的逻辑，无需调用这里
		return;

	UPxcAssistantSubsystem* pSystem = UPxcAssistantSubsystem::GetInstance();
	UFlowFenceActorWithKey* pObj = pSystem->GetObjectOfActorWithKey<UFlowFenceActorWithKey>(this, sKey);
	if (IsValid(pObj))
		pObj->OnCompleteAndFinished();
}

bool APxcGameState::MulticastFinishFlowFence_Validate(const FString& sKey)
{
	return true;
}