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
enum class ERandomZGNRoomType : uint8
{
	Entry = 0,
	Fight,
	Loot,
	Story,
	Shop,
};