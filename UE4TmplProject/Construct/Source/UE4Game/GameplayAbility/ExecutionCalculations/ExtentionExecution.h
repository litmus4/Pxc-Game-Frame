// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExtentionExecution.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API UExtentionExecution : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	FGameplayTag ExtentionVariableTag1;

	UPROPERTY()
	FGameplayTag ExtentionVariableTag2;

	UPROPERTY()
	FGameplayTag ExtentionVariableTag3;

	UExtentionExecution(const FObjectInitializer& ObjectInitializer);

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
		FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	//TODOJK 在ASC的OnAnyGameplayEffectRemovedDelegate()回调内部调用
	static void Discard(UAbilitySystemComponent* pASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);
};
