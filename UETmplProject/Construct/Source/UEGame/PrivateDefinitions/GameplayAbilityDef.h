// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayAbilityDef.generated.h"

UENUM(BlueprintType)
enum class EPassiveListeningType : uint8
{
	Unknown = 0,
	AttackExtra,
};

UENUM(BlueprintType)
enum class EDynamicFigureType : uint8
{
	Unknown = 0,
	AttackExtra,
};

UENUM(BlueprintType)
enum class EGEExtentionType : uint8
{
	Unknown = 0,
};