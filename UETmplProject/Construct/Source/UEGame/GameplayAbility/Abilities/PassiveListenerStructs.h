// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Framework/Structs/SignatureStructs.h"
#include "PrivateDefinitions/GameplayAbilityDef.h"
#include "PassiveLIstenerStructs.generated.h"

USTRUCT()
struct FPassiveListener : public FPointerModel
{
	GENERATED_BODY()
public:
	FPassiveListener() = default;
	FPassiveListener(const int64 xUid) : Super(xUid) {}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FPassiveListener::StaticStruct();
	}

	virtual EPtrModelBaseType GetBaseType() const override
	{
		return EPtrModelBaseType::PassiveListener;
	}

	virtual void Restart() {}
};

USTRUCT(BlueprintType)
struct FPassiveAttackExListener : public FPassiveListener
{
	GENERATED_BODY()
public:
	FPassiveAttackExListener() : iProcStep(0), fProcValue(0.0f) {}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float fAttackAdd;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float fAttackMulti;

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FPassiveAttackExListener::StaticStruct();
	}

	float Process(float fInValue, float fBaseValue);
	virtual void Restart() override;

	int32 iProcStep;
	float fProcValue;
};

USTRUCT()
struct FPassiveListenerGroup
{
	GENERATED_BODY()
public:
	FPassiveListenerGroup() : eType(EPassiveListeningType::Unknown) {}
	FPassiveListenerGroup(const FSharedSignature& Sig, EPassiveListeningType xType)
		: eType(xType) { tsetSignatures.Add(Sig); }

	UPROPERTY()
	TSet<FSharedSignature> tsetSignatures;

	UPROPERTY()
	EPassiveListeningType eType;
};

//暂时先放这里
USTRUCT(BlueprintType)
struct FDynamicFigureFeature
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPassiveListeningType ePassiveListeningType;
};