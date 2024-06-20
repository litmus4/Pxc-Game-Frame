// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ActorStructs.generated.h"

USTRUCT(BlueprintType)
struct FSubLevelLinkInfo
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<AActor> pDefaultTeleportDest;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FName> tarrNeighbors;
};