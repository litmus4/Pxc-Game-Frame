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
	Unknown,//并无Unknown
};

UENUM(BlueprintType)
enum class EPlayerUniEventType : uint8//FLAGJK 定义在Character相关的Def
{
	Unknown = 0,
};