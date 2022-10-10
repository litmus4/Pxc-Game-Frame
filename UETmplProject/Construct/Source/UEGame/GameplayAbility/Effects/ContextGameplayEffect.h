// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include <unordered_map>
#include "ContextGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UContextGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	typedef std::pair<FGameplayEffectContextHandle, FActiveGameplayEffectHandle> tPairContext;
	typedef std::unordered_map<UAbilitySystemComponent*, tPairContext> tMapContext;
	
public:
	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;

	void AddContextAndHandle(UAbilitySystemComponent* pASC,
		const FGameplayEffectContextHandle& xContext, const FActiveGameplayEffectHandle& xHandle);
	void RemoveContextPair(UAbilitySystemComponent* pASC);

	UFUNCTION(BlueprintPure, Category = Context)
	AActor* GetContextInstigator(UAbilitySystemComponent* pASC);

	UFUNCTION(BlueprintPure, Category = Spec)
	float GetEffectLevel(UAbilitySystemComponent* pASC);

protected:
	tMapContext m_mapContexts;
};