// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Systems/PXCycleSystem.h"
#include "../Structs/SystemStructs.h"
#include "RandomGameplaySystem.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API URandomGameplaySystem : public UPXCycleSystem
{
	GENERATED_BODY()
	
public://Zhang Gun Neural
	//UFUNCTION(BlueprintCallable, Category = RandomGameplay)
	//void InitZGNLayers();

	//UFUNCTION(BlueprintPure, Category = RandomGameplay)
	//void GetZGNLayers(TArray<FRandomZGNLayer>& tarrOutZGNLayers);

public:
	//virtual void Release() override;
};
