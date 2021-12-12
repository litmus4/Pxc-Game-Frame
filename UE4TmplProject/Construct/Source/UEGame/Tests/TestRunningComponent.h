// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Framework/Structs/SignatureStructs.h"
#include "TestRunningComponent.generated.h"

USTRUCT(BlueprintType)
struct FTestParameter : public FPointerModel
{
	GENERATED_BODY()
public:
	FTestParameter() = default;
	FTestParameter(const int64 xUid) : Super(xUid) {}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTestParameter::StaticStruct();
	}

	virtual EPtrModelBaseType GetBaseType() const override
	{
		return EPtrModelBaseType::TestParameter;
	}
};

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UEGAME_API UTestRunningComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTestRunningComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void RunCppTestNoParam() {}
	virtual void RunCppTestWithParam(const FSharedSignature& ParamSig) {}

	UFUNCTION(BlueprintNativeEvent)
	void RunBPTestNoParam();
	virtual void RunBPTestNoParam_Implementation() {}

	UFUNCTION(BlueprintNativeEvent)
	void RunBPTestWithParam(const FSharedSignature& ParamSig);
	virtual void RunBPTestWithParam_Implementation(const FSharedSignature& ParamSig) {}

	virtual bool IsWithParam() { return false; }
	virtual void MakeParameterByOverlappingActor(AActor* pActor, FSharedSignature& OutSig);
	virtual bool IsRunning() { return false; }
};
