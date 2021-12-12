// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncWaitActorWithKey.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActorWithKeyDelegate);

/**
 * 
 */
UCLASS()
class UEGAME_API UAsyncWaitActorWithKey : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	friend class UPxcAssistantSubsystem;

public:
	UAsyncWaitActorWithKey(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintAssignable)
	FActorWithKeyDelegate DeleFinished;

	UPROPERTY(BlueprintAssignable)
	FActorWithKeyDelegate DeleTimeOut;

	UFUNCTION(BlueprintCallable, Category = "Flow Control", meta = (BlueprintInternalUseOnly = "TRUE"))
	static UAsyncWaitActorWithKey* AsyncUtil_WaitActorWithKey(AActor* pActor, const FString& sKey, float fWaitTime);

protected:
	virtual void Activate() override;

	void OnEnded(bool bFinish);

	UPROPERTY()
	AActor* m_pActor;

	UPROPERTY()
	FString m_sKey;

	UPROPERTY()
	float m_fWaitTime;

	UPROPERTY()
	FTimerHandle m_Timer;

};
