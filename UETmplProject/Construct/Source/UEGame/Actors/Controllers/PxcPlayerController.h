// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PxcPlayerController.generated.h"

class APxcPlayerRole;
class UPrlInputMappingContext;

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
	virtual void OnPossess(APawn* pPawn) override;
	virtual void OnUnPossess() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<APxcPlayerRole> m_cPlayerRole;

	UPROPERTY(BlueprintReadOnly)
	APxcPlayerRole* m_pPlayerRole = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Input)
	UPrlInputMappingContext* m_pPrlInputMappingContext;
};
