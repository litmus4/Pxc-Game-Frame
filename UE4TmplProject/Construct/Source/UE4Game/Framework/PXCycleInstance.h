// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PXCycleInstance.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API UPXCycleInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;

private:
	void OnGameModeInitialized(AGameModeBase* pGM);
};
