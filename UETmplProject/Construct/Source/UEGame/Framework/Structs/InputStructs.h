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
