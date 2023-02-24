// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Roles/PxcLogicRole.h"
#include "PxcPlayerRole.generated.h"

class APxcPlayerCharacter;

/**
 * 
 */
UCLASS()
class UEGAME_API APxcPlayerRole : public APxcLogicRole
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure)
	APxcPlayerCharacter* GetThePlayerCharacter();

	void SetThePlayerCharacter(APxcPlayerCharacter* pCharacter);

protected:
	UPROPERTY(BlueprintReadOnly)
	APxcPlayerCharacter* m_pPlayerCharacter;
};
