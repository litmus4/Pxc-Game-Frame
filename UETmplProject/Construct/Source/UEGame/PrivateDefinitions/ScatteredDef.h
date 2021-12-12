// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ScatteredDef.generated.h"

UENUM(BlueprintType)
enum class EDynamicAssetsType : uint8
{
	InputKeyIcon,
};

UENUM(BlueprintType)
enum class EGamepadModifierKey : uint8
{
	Unknown,//����Unknown
};

UENUM(BlueprintType)
enum class EPlayerUniEventType : uint8//FLAGJK ������Character��ص�Def
{
	Unknown = 0,
};