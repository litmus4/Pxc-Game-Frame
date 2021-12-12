// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "PxcGameState.generated.h"

class APxcGameMode;

/**
 * 
 */
UCLASS()
class UEGAME_API APxcGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	APxcGameState();

	void OnSetToWorld(APxcGameMode* pGameMode);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnSetToWorld(APxcGameMode* pGameMode);

public:
	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastUniversal(const FName& TypeName, UObject* pParamObject, FSharedSignature ParamSig, bool bSync);
	virtual void MulticastUniversal_Implementation(const FName& TypeName,
		UObject* pParamObject, FSharedSignature ParamSig, bool bSync);
	virtual bool MulticastUniversal_Validate(const FName& TypeName,
		UObject* pParamObject, FSharedSignature ParamSig, bool bSync);

	UFUNCTION(BlueprintImplementableEvent)
	void OnBroadUniversal(const FName& TypeName, UObject* pParamObject, FSharedSignature ParamSig);

	UFUNCTION(NetMulticast, Reliable, WithValidation)
	void MulticastFinishFlowFence(const FString& sKey);
	virtual void MulticastFinishFlowFence_Implementation(const FString& sKey);
	virtual bool MulticastFinishFlowFence_Validate(const FString& sKey);
};
