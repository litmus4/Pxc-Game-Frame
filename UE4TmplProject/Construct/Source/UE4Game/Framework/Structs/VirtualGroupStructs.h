// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrivateDefinitions/LevelManagersDef.h"
#include "VirtualGroupStructs.generated.h"

USTRUCT()
struct FVirtGrpFeature
{
	GENERATED_BODY()
public:
	FVirtGrpFeature() = default;

	virtual EVirtualGroupUsage GetUsage() const
	{
		return EVirtualGroupUsage::Unknown;
	}

	FName GroupName;
};

USTRUCT(BlueprintType)
struct FVirtGrpRTDFeature : public FVirtGrpFeature
{
	GENERATED_BODY()
public:
	FVirtGrpRTDFeature() : fTimeDilation(1.0f),
		bRuntimeActorExclusiveMark(false) {}

	virtual EVirtualGroupUsage GetUsage() const override
	{
		return EVirtualGroupUsage::RelativeTimeDilation;
	}

	UPROPERTY(BlueprintReadWrite)
	float fTimeDilation;

	//���Ż��ã�Ϊfalseʱ����ʹ��IgnoreParent
	UPROPERTY(BlueprintReadWrite)
	bool bRuntimeActorExclusiveMark;
};