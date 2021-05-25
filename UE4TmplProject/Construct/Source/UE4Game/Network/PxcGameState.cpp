// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/PxcGameState.h"
#include "NetBlueprintLibrary.h"
#include "PrivateDefinitions/NetDef.h"

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

void APxcGameState::MulticastUniversal_Implementation(const FName& TypeName, UObject* pParamObject, bool bSync)
{
	OnBroadUniversal(TypeName, pParamObject);

	if (bSync)
	{//TODOJK Player�������
		//APxcCharacterPlayer* pPlayer = CastChecked<APxcCharacterPlayer>(UNetBlueprintLibrary::Net_GetLocalPlayerCharacter(this));
		//pPlayer->ServerSyncComplete(EServerSyncType::Universal, true);
	}
}

bool APxcGameState::MulticastUniversal_Validate(const FName& TypeName, UObject* pParamObject, bool bSync)
{
	return true;
}

void APxcGameState::MulticastFinishFlowFence_Implementation(const FString& sKey)
{
	if (GetWorld()->GetNetMode() != NM_Client)//�����������Լ����߼��������������
		return;

	//FLAGJK_Net
}

bool APxcGameState::MulticastFinishFlowFence_Validate(const FString& sKey)
{
	return true;
}