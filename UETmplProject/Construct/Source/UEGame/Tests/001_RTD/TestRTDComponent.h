// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tests/TestRunningComponent.h"
#include <vector>
#include "TestRTDComponent.generated.h"

USTRUCT(BlueprintType)
struct FTestRTDParameter : public FTestParameter
{
	GENERATED_BODY()
public:
	FTestRTDParameter() : pController(nullptr) {}

	UPROPERTY(BlueprintReadOnly)
	APlayerController* pController;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTestRTDParameter::StaticStruct();
	}
};

/**
 * 
 */
UCLASS(Blueprintable)
class UEGAME_API UTestRTDComponent : public UTestRunningComponent
{
	GENERATED_BODY()

public:
	UTestRTDComponent();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName m_GroupName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UCurveFloat* m_pCameraDynamic;

	UFUNCTION(BlueprintPure)
	bool CastParameter(const FSharedSignature& ParamSig, FTestRTDParameter& OutParam);
	
public:
	virtual void RunCppTestWithParam(const FSharedSignature& ParamSig) override;

	virtual bool IsWithParam() override { return true; }
	virtual void MakeParameterByOverlappingActor(AActor* pActor, FSharedSignature& OutSig) override;

protected:
	UFUNCTION()
	void OnCameraOrbitEnded();

	UFUNCTION()
	void OnCameraDilationEnded(bool bCanceled);

	UFUNCTION()
	void OnGroupDilationEnded(bool bCanceled);

	UFUNCTION()
	void OnSpawnedDilationEnded(bool bCanceled);

	UFUNCTION()
	void OnGlobalDilationEnded(bool bCanceled);

	void CheckFinal();

	UPROPERTY()
	AActor* m_pSpawnedShowActor;

	UPROPERTY()
	AActor* m_pCameraActor;

	UPROPERTY()
	APlayerController* m_pRunningController;

	std::vector<FTimerHandle> m_vecTimerCache;
	int32 m_iEndCount;
};
