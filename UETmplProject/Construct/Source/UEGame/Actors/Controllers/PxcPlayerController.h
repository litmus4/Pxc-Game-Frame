// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PxcPlayerController.generated.h"

class APxcPlayerRole;

/**
 * 
 */
UCLASS()
class UEGAME_API APxcPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	APxcPlayerRole* GetPlayerRole();

	void SpawnPlayerRole();

protected:
	UPROPERTY(BlueprintReadOnly)
	APxcPlayerRole* m_pPlayerRole;
};
