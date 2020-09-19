// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffect.h"
#include <list>
#include <functional>
#include "PxcAbilitySystemGlobals.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPxcGAS, Display, All);

USTRUCT()
struct FGEContModifyInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	UAbilitySystemComponent* pASC;

	UPROPERTY()
	const UGameplayEffect* pEffect;

	UPROPERTY()
	TMap<FGameplayAttribute, int32> tmapAttrToIndex;
};

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

	int32 FindGEContModifyFromQueue(UAbilitySystemComponent* pASC, const FGameplayAttribute& Attribute,
		std::function<bool(const FGameplayModifierInfo&)>&& fnQuery, const UGameplayEffect** ppOutEffect);
	void ClearRedundantGEContModify(bool bWarn = true);

	UPROPERTY(Transient, BlueprintReadOnly)
	const UGameplayEffect* m_pNowAppliedGE;

	std::list<FGEContModifyInfo> m_lisGEContModifyQueue;
};
