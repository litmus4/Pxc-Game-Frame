// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "EnhancedActionKeyMapping.h"
#include <functional>
#include "PxcNativeLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UPxcNativeLibrary : public UObject
{
	GENERATED_BODY()
	
};

struct FPxcNativeLibrary
{
	FPxcNativeLibrary() = delete;

	static void GetAllActorsInSubLevel(UWorld* pWorld, ULevelStreaming* pLevelStreaming, TSubclassOf<AActor> Class, TArray<AActor*>& tarrOutActors,
		std::function<bool(AActor*)> fnFilter = [](AActor* pActor) { return true; });

	static uint8 EI_GetModifierCodeFromTriggers(const TArray<UInputTrigger*>& tarrTriggers);
	static bool EI_IsPositiveFromModifiers(const TArray<UInputModifier*>& tarrModifiers);
	static void EI_GetTriggersFromModifierCode(uint8 uModifierCode, TArray<TObjectPtr<UInputTrigger>>& tarrOutTriggers);
	static void EI_GetModifiersFromPositive(bool bPositive, TArray<TObjectPtr<UInputModifier>>& tarrOutModifiers);

	static bool EI_IsMappingEqual(const FEnhancedActionKeyMapping& Mapping1, const FEnhancedActionKeyMapping& Mapping2);
	static int32 EI_AddUniqueMapping(TArray<FEnhancedActionKeyMapping>& tarrMappings, const FEnhancedActionKeyMapping& Mapping);
};