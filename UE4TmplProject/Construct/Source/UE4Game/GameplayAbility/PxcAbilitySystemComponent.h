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
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityGivenDelegate, const FGameplayAbilitySpec&, AbilitySpec);
	
public:
	virtual int32 RemoveActiveEffects(const FGameplayEffectQuery& Query, int32 iStacksToRemove = -1);

	UPROPERTY(BlueprintAssignable)
	FAbilityGivenDelegate DeleAbilityGiven;

	UPROPERTY(BlueprintAssignable)
	FAbilityGivenDelegate DeleAbilityRemoved;

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnGiveAbility(const FGameplayAbilitySpec& AbilitySpec);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnRemoveAbility(const FGameplayAbilitySpec& AbilitySpec);

	//m_tmapPassiveListeners FLAGJK
};
