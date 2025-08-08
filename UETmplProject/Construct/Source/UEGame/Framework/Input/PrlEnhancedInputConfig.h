// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Structs/InputStructs.h"
#include "PrlEnhancedInputConfig.generated.h"

class UPrlInputMappingContext;

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

	void ReadTo(UPrlInputMappingContext* pPrlIMC) const;
	void WriteFrom(const UPrlInputMappingContext* pPrlIMC);
	FORCEINLINE bool IsValidConfig() const;

	void LoadConfigMappings(const FString& sConfigPath);
	void SaveConfigMappings();

	FString m_sConfigPath;
};
