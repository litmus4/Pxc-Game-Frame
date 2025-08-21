// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "InputTriggers.h"
#include "InputStructs.generated.h"

/**
 * 重写一个弦操作触发器
 */
UCLASS(MinimalAPI, NotBlueprintable, meta = (DisplayName = "Parallel Chorded Action", NotInputConfigurable = "true"))
class UInputTriggerPrlChordAction : public UInputTriggerChordAction
{
	GENERATED_BODY()

public:
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:
	virtual ETriggerState UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime) override;

public:

	// The action that must be triggering for this trigger's action to trigger
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger Settings", meta = (DisplayThumbnail = "false"))
	TObjectPtr<const UInputAction> CombinedChordAction = nullptr;
};

USTRUCT()
struct FPrlActionMappingConfig
{
	GENERATED_BODY()
public:
	FPrlActionMappingConfig(const FName& xActionName = NAME_None, const FKey& xKey = EKeys::Invalid, uint8 xChordCode = 0)
		: ActionName(xActionName), Key(xKey), uChordCode(xChordCode)
	{
	}

	UPROPERTY()
	FName ActionName;

	UPROPERTY()
	FKey Key;

	UPROPERTY()
	uint8 uChordCode;

	bool operator==(const FPrlActionMappingConfig& Other) const;
	bool operator<(const FPrlActionMappingConfig& Other) const;
};

USTRUCT()
struct FPrlAxisMappingConfig
{
    GENERATED_BODY()
public:
	FPrlAxisMappingConfig(const FName& xAxisName = NAME_None, const FKey& xKey = EKeys::Invalid, bool xNegative = false)
		: AxisName(xAxisName), Key(xKey), bNegative(xNegative)
	{
	}

	UPROPERTY()
	FName AxisName;

	UPROPERTY()
	FKey Key;

	UPROPERTY()
	bool bNegative = false;

	bool operator==(const FPrlAxisMappingConfig& Other) const;
	bool operator<(const FPrlAxisMappingConfig& Other) const;
};

USTRUCT()
struct FPrlInputActionEx
{
	GENERATED_BODY()
public:
	UPROPERTY()
	const UInputAction* pAction = nullptr;

	ETriggerEvent eMainBindedEvent = ETriggerEvent::None;
	ETriggerEvent eExtraBindedEvent = ETriggerEvent::None;
};