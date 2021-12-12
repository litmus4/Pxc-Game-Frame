// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <set>
#include "UtilityStructs.generated.h"

DECLARE_DYNAMIC_DELEGATE(FSimpleDynamicDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSimpleDynMultiDelegate);

USTRUCT(BlueprintType)
struct FSimpleDynamicDelexMono
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSimpleDynamicDelegate DeleCall;
};

USTRUCT(BlueprintType)
struct FSimpleDynamicDelexPair
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSimpleDynamicDelegate DeleSucceeded;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSimpleDynamicDelegate DeleFailed;
};

USTRUCT(BlueprintType)
struct FIdentifiedLessStack
{
	GENERATED_BODY()
public:
	int32 PushValue(float fValue);
	void PopValue(int32 iUid);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString sTypeKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fDiffPrecision = 0.000001f;

	std::set<float> set;
	TMap<int32, std::set<float>::iterator> tmapUidToIt;
};

USTRUCT(BlueprintType)
struct FIdentifiedGreaterStack
{
	GENERATED_BODY()
public:
	int32 PushValue(float fValue);
	void PopValue(int32 iUid);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FString sTypeKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fDiffPrecision = 0.000001f;

	typedef std::set<float, std::greater<float>> tSetGreater;
	tSetGreater set;
	TMap<int32, tSetGreater::iterator> tmapUidToIt;
};