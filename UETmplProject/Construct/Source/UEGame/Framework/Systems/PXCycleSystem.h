// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PXCycleSystem.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UPXCycleSystem : public UObject
{
	GENERATED_BODY()
	
public:
	virtual void Tick(float fDeltaSeconds);
	virtual void Release();
};
