// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tests/TestRunningComponent.h"
#include <vector>
#include "TestGroupCentralComponent.generated.h"

USTRUCT(BlueprintType)
struct FTestGroupCentralParameter : public FTestParameter
{
	GENERATED_BODY()
public:
	FTestGroupCentralParameter()
		: pMainActor(nullptr), pController(nullptr) {}

	UPROPERTY(BlueprintReadOnly)
	AActor* pMainActor;

	UPROPERTY(BlueprintReadOnly)
	APlayerController* pController;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTestGroupCentralParameter::StaticStruct();
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class UE4GAME_API UTestGroupCentralComponent : public UTestRunningComponent
{
	GENERATED_BODY()
	
public:
	UTestGroupCentralComponent();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> m_cSpawnedDirectActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AActor> m_cCameraActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName m_GroupName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* m_pDefaultDynamic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* m_pOwnerDynamic;

	UFUNCTION(BlueprintPure)
	bool CastParameter(const FSharedSignature& ParamSig, FTestGroupCentralParameter& OutParam);

public:
	virtual void RunCppTestWithParam(const FSharedSignature& ParamSig) override;

	virtual void TickComponent(float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* pThisTickFunction) override;

protected:
	UPROPERTY()
	AActor* m_pSpawnedDirectActor;

	UPROPERTY()
	AActor* m_pCameraActor;

	std::vector<FTimerHandle> m_vecTimerCache;
};
//FLAGJK ÒÆ¶¯