// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrivateDefinitions/MainDef.h"
#include "SystemStructs.generated.h"

USTRUCT(BlueprintType)
struct FRandomSubLevelList
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowedClasses = "World"))
	TArray<FSoftObjectPath> tarrResList;

	UPROPERTY(BlueprintReadOnly)
	TArray<TSoftObjectPtr<UWorld>> tarrLevels;
};

USTRUCT(BlueprintType)
struct FRandomZGNNode
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	ERandomZGNRoomType eType;

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> tarrNextRoomLinks;

	UPROPERTY(BlueprintReadOnly)
	TArray<int32> tarrBackRoomLinks;
};

USTRUCT(BlueprintType)
struct FRandomZGNLayer
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	TArray<FRandomZGNNode> tarrNodes;
};
