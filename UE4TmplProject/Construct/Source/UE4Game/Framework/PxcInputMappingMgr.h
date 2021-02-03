// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/InputSettings.h"
#include "Structs/ScatteredStructs.h"
#include "PxcInputMappingMgr.generated.h"

USTRUCT(BlueprintType)
struct FInputMappingQuad
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	FPxcInputMapping GamepadInputMapping;

	UPROPERTY(BlueprintReadOnly)
	FPxcInputMapping KeyboardInputMapping;

	UPROPERTY(BlueprintReadOnly)
	FPxcAxputMapping GamepadAxputMapping;

	UPROPERTY(BlueprintReadOnly)
	FPxcAxputMapping KeyboardAxputMapping;

	UPROPERTY(BlueprintReadOnly)
	bool bAxis;

	UPROPERTY()
	FName UniActionName;
};

/**
 * 
 */
UCLASS()
class UE4GAME_API UPxcInputMappingMgr : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Input)
	void LoadInputMappings();

	UFUNCTION(BlueprintCallable, Category = Input)
	void ReleaseInputMappings();

	UFUNCTION(BlueprintCallable, Category = Input)
	TArray<FInputMappingQuad> GetInputMappings();

	UFUNCTION(BlueprintPure, Category = Input)
	FString GetDisplayNameOfAction(const FName& ActionName, bool bAxisPositiveDir);

private:
	UPROPERTY(Transient)
	TArray<FInputMappingQuad> m_tarrInputMappings;
};
