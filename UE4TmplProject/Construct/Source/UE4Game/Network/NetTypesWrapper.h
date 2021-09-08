// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Framework/Structs/SignatureStructs.h"
#include "GameplayTagContainer.h"
#include "NetTypesWrapper.generated.h"

USTRUCT()
struct FNetParamWrapper : public FPointerModel
{
	GENERATED_BODY()
public:
	FNetParamWrapper() = default;
	FNetParamWrapper(const int64 xUid) : Super(xUid) {}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNetParamWrapper::StaticStruct();
	}

	virtual EPtrModelBaseType GetBaseType() const override
	{
		return EPtrModelBaseType::NetParamWrapper;
	}
};

USTRUCT(BlueprintType)
struct FNetIntegerParamWrapper : public FNetParamWrapper
{
	GENERATED_BODY()
public:
	FNetIntegerParamWrapper() : iData(0) {}
	FNetIntegerParamWrapper(int32 xData) : iData(xData) {}

	UPROPERTY(BlueprintReadWrite)
	int32 iData;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNetIntegerParamWrapper::StaticStruct();
	}
};

USTRUCT(BlueprintType)
struct FNetBoolParamWrapper : public FNetParamWrapper
{
	GENERATED_BODY()
public:
	FNetBoolParamWrapper() : bData(false) {}
	FNetBoolParamWrapper(bool xData) : bData(xData) {}

	UPROPERTY(BlueprintReadWrite)
	bool bData;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNetBoolParamWrapper::StaticStruct();
	}
};

USTRUCT(BlueprintType)
struct FNetObjectParamWrapper : public FNetParamWrapper
{
	GENERATED_BODY()
public:
	FNetObjectParamWrapper() : pObject(nullptr) {}
	FNetObjectParamWrapper(UObject* xObject) : pObject(xObject) {}

	UPROPERTY(BlueprintReadWrite)
	UObject* pObject;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNetObjectParamWrapper::StaticStruct();
	}
};

USTRUCT(BlueprintType)
struct FNetClassParamWrapper : public FNetParamWrapper
{
	GENERATED_BODY()
public:
	FNetClassParamWrapper() = default;
	FNetClassParamWrapper(UClass* pClass) : cClass(pClass) {}

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UObject> cClass;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNetClassParamWrapper::StaticStruct();
	}
};

USTRUCT(BlueprintType)
struct FNetGameplayTagParamWrapper : public FNetParamWrapper
{
	GENERATED_BODY()
public:
	FNetGameplayTagParamWrapper() = default;
	FNetGameplayTagParamWrapper(const FGameplayTag& xTag) : Tag(xTag) {}

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag Tag;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNetGameplayTagParamWrapper::StaticStruct();
	}
};

USTRUCT(BlueprintType)
struct FNetTreeParamWrapper : public FNetParamWrapper
{
	GENERATED_BODY()
public:
	FNetTreeParamWrapper() = default;
	FNetTreeParamWrapper(const TArray<FSharedSignature>& xChildren)
		: tarrChildren(xChildren) {}

	UPROPERTY(BlueprintReadWrite)
	TArray<FSharedSignature> tarrChildren;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FNetTreeParamWrapper::StaticStruct();
	}
};