// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VirtualGroupStructs.generated.h"

UENUM()
enum class EVirtualGroupUsage : uint8
{
	Unknown = 0,
	RelativeTimeDilation,
};

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
};

USTRUCT()
struct FVirtGrpRTDFeature : public FVirtGrpFeature
{
	GENERATED_BODY()
public:
	FVirtGrpRTDFeature() : fTimeDilation(1.0f) {}

	virtual EVirtualGroupUsage GetUsage() const override
	{
		return EVirtualGroupUsage::RelativeTimeDilation;
	}

	UPROPERTY()
	float fTimeDilation;
};