// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "../Utilities/UtilityStructs.h"
#include "PxcBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API UPxcBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GameplayEffects|Extention")
	static FActiveGameplayEffectHandle ApplyExtentionGESpecWithReturn(UAbilitySystemComponent* pASC, const FGameplayEffectSpecHandle& SpecHandle,
		FActiveGameplayEffectHandle StackedHandle, TMap<FString, float>& tmapRet);

	UFUNCTION(BlueprintCallable, Category = "GameplayEffects|Extention")
	static void DiscardExtentionGEByStackedHandle(UAbilitySystemComponent* pASC, FActiveGameplayEffectHandle StackedHandle);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static int32 PushIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static void PopIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, int32 iUid);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static int32 PushIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static void PopIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, int32 iUid);
};
