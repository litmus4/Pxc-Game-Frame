// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterTrajectoryComponent.h"
#include "PxcCharacterTrajectoryComponent.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, meta = (BlueprintSpawnableComponent))
class UEGAME_API UPxcCharacterTrajectoryComponent : public UCharacterTrajectoryComponent
{
	GENERATED_BODY()
	
public:
	FPoseSearchQueryTrajectory GetTrajectory() const;
};
