// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tests/TestRunningComponent.h"
#include "TestRTDComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class UE4GAME_API UTestRTDComponent : public UTestRunningComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> m_cSpawnedShowActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform m_transSpawnedShowing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> m_cCameraActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FTransform m_transCamera;
	
public:
	virtual void RunCppTestNoParam() override;

protected:
	UPROPERTY()
	AActor* m_pSpawnedShowActor;

	UPROPERTY()
	AActor* m_pCameraActor;
};
