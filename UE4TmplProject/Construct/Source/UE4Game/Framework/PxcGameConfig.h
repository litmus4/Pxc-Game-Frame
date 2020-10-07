// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayAbility/PxcAbilitySystemComponent.h"
#include "PxcGameConfig.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, BlueprintType, NotBlueprintable)
class UE4GAME_API UPxcGameConfig : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = GameplayEffect)
	FGameplayTagContainer UnlockGameplayEffectParentTags;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TMap<EDynamicFigureType, FDynamicFigureFeature> tmapDynamicFigureToFeature;
};
