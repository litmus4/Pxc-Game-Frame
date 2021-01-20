// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GameplayAbility/PxcAbilitySystemComponent.h"
#include "Framework/Structs/ScatteredStructs.h"
#include "PxcGameConfig.generated.h"

/**
 * 
 */
UCLASS(Config = Game, DefaultConfig, BlueprintType, NotBlueprintable)
class UE4GAME_API UPxcGameConfig : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = StaticResource)
	TSoftObjectPtr<UDataTable> GEExtentionContToInstUDT;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = GameplayEffect)
	FGameplayTagContainer UnlockGameplayEffectParentTags;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TMap<EDynamicFigureType, FDynamicFigureFeature> tmapDynamicFigureToFeature;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Input)
	TArray<FGamepadCombineMapping> tarrGamepadCombineMappings;

	void AddGamepadCombineMapping(const FGamepadCombineMapping& CombineMapping);
	void RemoveGamepadCombineMapping(const FGamepadCombineMapping& CombineMapping);

	const TMap<FName, TArray<FGamepadCombineMapping>>& GetGamepadCombineMappings() const;

private:
	bool bGamepadCombineDirty = false;
};
