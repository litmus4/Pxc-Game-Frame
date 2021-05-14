// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tests/TestRunningComponent.h"
#include "TestRTDComponent.generated.h"

USTRUCT(BlueprintType)
struct FTestRTDParameter : public FTestParameter
{
	GENERATED_BODY()
public:
	FTestRTDParameter() : pController(nullptr) {}

	UPROPERTY(BlueprintReadOnly)
	AController* pController;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTestRTDParameter::StaticStruct();
	}
};

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

	UFUNCTION(BlueprintPure)
	bool CastParameter(const FSharedSignature& ParamSig, FTestRTDParameter& OutParam);
	
public:
	virtual void RunCppTestWithParam(const FSharedSignature& ParamSig) override;

	virtual bool IsWithParam() override { return true; }
	virtual void MakeParameterByOverlappingActor(AActor* pActor, FSharedSignature& OutSig) override;

protected:
	UPROPERTY()
	AActor* m_pSpawnedShowActor;

	UPROPERTY()
	AActor* m_pCameraActor;
};
