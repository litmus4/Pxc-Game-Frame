// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "PxcEngineSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UPxcEngineSubsystem : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	UFUNCTION()
	void OnPostEngineInit();

	FDelegateHandle DelePostEngineInitHandle;
};
