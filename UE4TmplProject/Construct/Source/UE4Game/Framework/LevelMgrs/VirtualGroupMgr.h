// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VirtualGroupMgr.generated.h"

USTRUCT()
struct FVirtualGroupData
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FName Name;

	UPROPERTY()
	TArray<AActor*> tarrActors;

	//
};

/**
 * 
 */
UCLASS()
class UE4GAME_API UVirtualGroupMgr : public UObject
{
	GENERATED_BODY()
	
public:

private:
	//
};
