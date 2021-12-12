// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/DataTable.h"
#include "GameplayEffect.h"
#include "UDTRowStructs.generated.h"

USTRUCT(BlueprintInternalUseOnly)
struct UEGAME_API FUTRGExContToInst : public FTableRowBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag ExtentionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> InstantEffect;
};