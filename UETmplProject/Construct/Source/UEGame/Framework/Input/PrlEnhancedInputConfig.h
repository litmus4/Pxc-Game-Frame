// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Structs/InputStructs.h"
#include "PrlEnhancedInputConfig.generated.h"

/**
 * 
 */
UCLASS(Config = PrlEnhancedInput, DefaultConfig)
class UEGAME_API UPrlEnhancedInputConfig : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config)
	TArray<FPrlActionMappingConfig> m_tarrActionMappings;

	UPROPERTY(Config)
    TArray<FPrlAxisMappingConfig> m_tarrAxisMappings;

	void SaveConfigMappings();

	//FLAGJK_Now Read Write
};
