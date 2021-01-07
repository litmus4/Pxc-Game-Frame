// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <set>
#include "UtilityStructs.generated.h"

USTRUCT(BlueprintType)
struct FIdentifiedLessStack
{
	GENERATED_BODY()
public:
	int32 PushValue(float fValue);
	void PopValue(int32 iUid);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 iTypeKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fDiffPrecision = 0.000001f;

	std::set<float> set;
	TMap<int32, std::set<float>::iterator> tmapUidToIt;
};

UCLASS(BlueprintType, NotBlueprintable)
class UIdentifiedLessBox : public UObject
{
	GENERATED_BODY()
public:
	UIdentifiedLessBox() = default;
	UIdentifiedLessBox(FIdentifiedLessStack* pStack)
		: m_pStack(pStack) {}

	FIdentifiedLessStack* m_pStack;
};

USTRUCT(BlueprintType)
struct FIdentifiedGreaterStack
{
	GENERATED_BODY()
public:
	int32 PushValue(float fValue);
	void PopValue(int32 iUid);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 iTypeKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float fDiffPrecision = 0.000001f;

	typedef std::set<float, std::greater<float>> tSetGreater;
	tSetGreater set;
	TMap<int32, tSetGreater::iterator> tmapUidToIt;
};

UCLASS(BlueprintType, NotBlueprintable)
class UIdentifiedGreaterBox : public UObject
{
	GENERATED_BODY()
public:
	UIdentifiedGreaterBox() = default;
	UIdentifiedGreaterBox(FIdentifiedGreaterStack* pStack)
		: m_pStack(pStack) {}

	FIdentifiedGreaterStack* m_pStack;
};