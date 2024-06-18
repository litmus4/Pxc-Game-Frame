// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MainDef.generated.h"

UENUM(BlueprintType)
enum class ECycleSystemType : uint8
{
	InputMapping,
	RandomGameplay,
};

UENUM(BlueprintType)
enum class EGamepadModifierKey : uint8
{
	Unknown,//²¢ÎÞUnknown
};

UENUM(BlueprintType)
enum class ERandomModeType : uint8
{
	Unknown = 0,
	ZGN,
};

UENUM(BlueprintType)
enum class ERandomZGNRoomType : uint8
{
	Entry = 0,
	Stage,
	Bonus,
	Story,
	Support,
};