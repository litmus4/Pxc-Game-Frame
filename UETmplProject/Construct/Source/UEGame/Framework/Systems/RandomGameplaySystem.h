// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Systems/PXCycleSystem.h"
#include "../Structs/SystemStructs.h"
#include "RandomGameplaySystem.generated.h"

class URandomModeComponent;

/**
 * 
 */
UCLASS()
class UEGAME_API URandomGameplaySystem : public UPXCycleSystem
{
	GENERATED_BODY()
	
public://Zhang Gun Neural
	UFUNCTION(BlueprintCallable, Category = RandomGameplay)
	void GenerateZGNLayers(TSubclassOf<URandomModeComponent> cComponent);

	UFUNCTION(BlueprintPure, Category = RandomGameplay)
	void GetZGNLayers(TArray<FRandomZGNLayer>& tarrOutZGNLayers);

	UFUNCTION(BlueprintCallable, Category = RandomGameplay)
	void ClearZGNLayers();

public:
	virtual void Release() override;

private:
	void VerifyZGNComponent(URandomModeComponent* pComp);

	UPROPERTY()
	TArray<FRandomZGNLayer> m_tarrZGNLayers;
};
