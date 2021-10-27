// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PrivateDefinitions/MainDef.h"
#include <list>
#include "PXCycleInstance.generated.h"

class UPXCycleSystem;
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
	virtual bool Tick(float fDeltaSeconds);
	virtual void Shutdown() override;

	UPROPERTY(BlueprintReadWrite)
	bool m_bKeyboardRuntime = true;

	UFUNCTION(BlueprintPure)
	UPXCycleSystem* GetCycleSystem(ECycleSystemType eType);

	UFUNCTION(BlueprintPure)
	UPxcInputMappingMgr* GetInputMappingMgr();

private:
	void AddSystem(ECycleSystemType eType, UPXCycleSystem* pSystem);
	void ReleaseSystems(ECycleSystemType* pTypes, int32 iTypeNum);
	void OnGameModeInitialized(AGameModeBase* pGM);
	void OnPreClientTravel(const FString& sPendingURL, ETravelType eTravelType, bool bIsSeamlessTravel);

	UPROPERTY()
	TMap<ECycleSystemType, UPXCycleSystem*> m_tmapSystems;

	std::list<UPXCycleSystem*> m_lisSystems;
	FDelegateHandle DeleTickHandle;
};
