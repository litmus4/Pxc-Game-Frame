// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LevelManagersDef.generated.h"

UENUM(BlueprintType)
enum class EVirtualGroupUsage : uint8
{
	Unknown = 0,
	RelativeTimeDilation,
	CentralTarget,
};

UENUM(BlueprintType)
enum class ESpecialLevelMgrType : uint8
{
	RelativeTimeDilation,
	CentralTarget,
};

UENUM(BlueprintType)
enum class ERTDilationLevel : uint8
{
	Global,
	AffectGroup,
	AffectActor,
	MAX
};