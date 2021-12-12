// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "NetDef.generated.h"

UENUM()
enum class EServerSyncType : uint8
{
	Universal,
	FlowFence,
};