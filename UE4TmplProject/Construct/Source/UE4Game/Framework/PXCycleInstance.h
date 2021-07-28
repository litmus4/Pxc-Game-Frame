// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PXCycleInstance.generated.h"

class UPxcInputMappingMgr;

/**
 * 
 */
UCLASS()
class UE4GAME_API UPXCycleInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual void Shutdown() override;

	UPROPERTY(BlueprintReadWrite)
	bool m_bKeyboardRuntime = true;

	UFUNCTION(BlueprintPure)
	UPxcInputMappingMgr* GetInputMappingMgr();

private:
	void OnGameModeInitialized(AGameModeBase* pGM);
	void OnPreClientTravel(const FString& sPendingURL, ETravelType eTravelType, bool bIsSeamlessTravel);

	UPROPERTY()
	UPxcInputMappingMgr* m_pInputMappingMgr;
};
