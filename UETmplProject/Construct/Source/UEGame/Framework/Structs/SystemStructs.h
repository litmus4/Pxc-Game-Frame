// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/PlayerInput.h"
#include "PrivateDefinitions/MainDef.h"
#include "SystemStructs.generated.h"

USTRUCT(BlueprintType)
struct FGamepadCombineMapping
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FKey MainKey;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EGamepadModifierKey eModifierKey;

	bool operator==(const FGamepadCombineMapping& Other) const;
};

USTRUCT(BlueprintType)
struct FPxcInputMapping
{
	GENERATED_BODY()
public:
	FPxcInputMapping() : uModifierCode(0) {}
	FPxcInputMapping(const FName& xActionName);
	FPxcInputMapping(const FName& xActionName, bool bDevisedKeyboard, int32 iIndex);

	void InitMapping(bool* pbDevisedKeyboard = nullptr, int32 iIndex = -1);
	bool IsSame(const FPxcInputMapping& Other);
	bool IsSame(const struct FPxcAxputMapping& Other);

	UPROPERTY(BlueprintReadOnly)
	FName ActionName;

	UPROPERTY(BlueprintReadOnly)
	FName KeyName;

	UPROPERTY(BlueprintReadOnly)
	uint8 uModifierCode;
};

USTRUCT(BlueprintType)
struct FPxcAxputMapping
{
	GENERATED_BODY()
public:
	FPxcAxputMapping() : bPositiveDir(true), iFloatPositive(0) {}
	FPxcAxputMapping(const FName& xAxisName, bool bPositive);
	FPxcAxputMapping(const FName& xAxisName, bool bPositive, bool bDevisedKeyboard, int32 iIndex);

	void InitMapping(bool* pbDevisedKeyboard = nullptr, int32 iIndex = -1);
	bool IsSame(const FPxcInputMapping& Other);

	UPROPERTY(BlueprintReadOnly)
	FName AxisName;

	UPROPERTY(BlueprintReadOnly)
	bool bPositiveDir;

	UPROPERTY(BlueprintReadOnly)
	FName KeyName;

	UPROPERTY(BlueprintReadOnly)
	int32 iFloatPositive;//!=0Ê±Ö»ÊÖ±ú
};

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
	ERandomZGNRoomSubType eSubType;

	UPROPERTY(BlueprintReadOnly)
	TArray<ERandomZGNRoomType> tarrRandomTypes;

	UPROPERTY(BlueprintReadOnly)
	float fRandomMultiper;

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

USTRUCT(BlueprintType)
struct FRandomZGNRoomState
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadOnly)
	ERandomZGNRoomSubType eSubType;

	UPROPERTY(BlueprintReadOnly)
	ERandomZGNRoomType eRandomType;

	UPROPERTY(BlueprintReadOnly)
	float fRandomMultiper;
};