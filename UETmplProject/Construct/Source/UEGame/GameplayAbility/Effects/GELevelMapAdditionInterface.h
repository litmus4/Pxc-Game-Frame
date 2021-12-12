// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GELevelMapAdditionInterface.generated.h"

class UAbilitySystemComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UGELevelMapAdditionInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UEGAME_API IGELevelMapAdditionInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GetAdditionInCurrentLevel(UAbilitySystemComponent* pASC, TMap<FString, float>& tmapOutAddition);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
	void GetAdditionByLevel(UAbilitySystemComponent* pASC, int32 iLevel, TMap<FString, float>& tmapOutAddition);
};
