// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrivateDefinitions/ScatteredDef.h"
#include "ScatteredStructs.generated.h"

USTRUCT()
struct FPxcSeat
{
	GENERATED_BODY()
};

struct FName4Stl
{
public:
	FName4Stl() = default;
	FName4Stl(const FName& x) : N(x) {}

	inline bool operator<(const FName4Stl& Other) const
	{
		return N.FastLess(Other.N);
	}

	FName N;
};