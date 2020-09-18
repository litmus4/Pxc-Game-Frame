// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "PxcAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API UPxcAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	virtual void InitGlobalData() override;
	virtual void SetCurrentAppliedGE(const FGameplayEffectSpec* pSpec) override;

	UPROPERTY(Transient, BlueprintReadOnly)
	const UGameplayEffect* m_pNowAppliedGE;
};
