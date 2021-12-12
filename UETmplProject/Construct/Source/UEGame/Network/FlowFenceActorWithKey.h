// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Utilities/AsyncWaitActorWithKey.h"
#include "FlowFenceActorWithKey.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UFlowFenceActorWithKey : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	friend class UPxcAssistantSubsystem;
	friend class APxcGameState;

public:
	UFlowFenceActorWithKey(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintAssignable)
	FActorWithKeyDelegate DeleFinished;

	UPROPERTY(BlueprintAssignable)
	FActorWithKeyDelegate DeleTimeOut;

	UFUNCTION(BlueprintCallable, Category = "Flow Control", meta = (WorldContext = "pWCO", BlueprintInternalUseOnly = "TRUE"))
	static UFlowFenceActorWithKey* StartFlowFence(UObject* pWCO, FName CheckName, float fWaitTime);

protected:
	virtual void Activate() override;

	UFUNCTION()
	void OnCompleteAndFinished();

	UFUNCTION()
	void OnCompleteAndTimeOut();

	void OnEnded(bool bFinish);

	UPROPERTY()
	FName m_CheckName;

	UPROPERTY()
	float m_fWaitTime;

	UPROPERTY()
	FTimerHandle m_ClientTimer;
};
