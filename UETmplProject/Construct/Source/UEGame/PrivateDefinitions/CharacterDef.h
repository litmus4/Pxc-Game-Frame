// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CharacterDef.generated.h"

UENUM(BlueprintType)
enum class EPlayerUniEventType : uint8
{
	Unknown = 0,
};

UENUM(BlueprintType)
enum class EAnimBPType : uint8
{
	DirectionTurned,
	ControllerTurned,
	MotionMatchingDT,
	MotionMatchingCT
};

UENUM(BlueprintType)
enum class EMotionState : uint8
{
	Idle = 0,
	StartMove,
	Move,
	EndMove
};

UENUM(BlueprintType)
enum class EEndMoveFootType : uint8
{
	Any = 0,
	Left,
	Right
};