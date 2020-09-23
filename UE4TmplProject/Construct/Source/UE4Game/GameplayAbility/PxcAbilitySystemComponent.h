// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PxcAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API UPxcAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:
	virtual int32 RemoveActiveEffects(const FGameplayEffectQuery& Query, int32 iStacksToRemove = -1);
};
