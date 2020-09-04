// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SignatureStructs.generated.h"

UENUM(BlueprintType)
enum class EPtrModelBaseType : uint8
{
	Model = 0,
};

USTRUCT()
struct FPointerModel
{
	GENERATED_BODY()
public:
	FPointerModel() : lUid(-1LL), bFake(false) {}
	FPointerModel(const int64 xUid)
		: lUid(xUid), bFake(true) {}
	virtual ~FPointerModel() {}

	void GenerateUidLocal();

	friend uint32 GetTypeHash(const FPointerModel& Model);

	virtual UScriptStruct* GetScriptStruct() const
	{
		return FPointerModel::StaticStruct();
	}

	virtual EPtrModelBaseType GetBaseType()
	{
		return EPtrModelBaseType::Model;
	}

	UPROPERTY()
	int64 lUid;

	UPROPERTY()
	bool bFake;
};

USTRUCT(BlueprintType)
struct FSharedSignature
{
	GENERATED_BODY()
public:
	FSharedSignature() = default;
	FSharedSignature(FPointerModel* xModel);
	FSharedSignature(const FSharedSignature& OtherSig);
	FSharedSignature(const FPointerModel& xModel);

	FORCEINLINE bool IsValid() const { return pModel.IsValid(); }
	FORCEINLINE const FPointerModel* Get() const { return pModel.Get(); }
	FORCEINLINE FPointerModel* Get() { return pModel.Get(); }
	FORCEINLINE FPointerModel* operator->() const { return pModel.Get(); }
	FORCEINLINE void Clear() { pModel.Reset(); }
	//FLAGJK

private:
	TSharedPtr<FPointerModel> pModel;
};