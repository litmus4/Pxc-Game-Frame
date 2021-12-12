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
class UEGAME_API UPxcGameConfig : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = StaticResource)
	TSoftObjectPtr<UDataTable> GEExtentionContToInstUDT;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = GameplayEffect)
	FGameplayTagContainer UnlockGameplayEffectParentTags;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Gameplay)
	TMap<EDynamicFigureType, FDynamicFigureFeature> tmapDynamicFigureToFeature;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Assets)
	TMap<EDynamicAssetsType, FString> tmapDynamicAssetsPathes;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Input)
	TArray<FGamepadCombineMapping> tarrGamepadCombineMappings;

	void AddGamepadCombineMapping(const FGamepadCombineMapping& CombineMapping);
	void RemoveGamepadCombineMapping(const FGamepadCombineMapping& CombineMapping);

	const TMap<FName, TArray<FGamepadCombineMapping>>& GetGamepadCombineMappings() const;

public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Story)
	int32 iQuestFsmMaxStateNum = 10;

	UPROPERTY(Config, EditAnywhere, BlueprintReadOnly, Category = Story)
	int32 iQuestFsmSubCutStateNum = 6;

private:
	bool bGamepadCombineDirty = false;
};
