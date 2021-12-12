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
class UEGAME_API UPXCycleInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	virtual void Init() override;
	virtual bool Tick(float fDeltaSeconds);
	virtual void Shutdown() override;

	void AddSystem(ECycleSystemType eType, UPXCycleSystem* pSystem, bool bTick = true);
	void ReleaseSystems(ECycleSystemType* pTypes, int32 iTypeNum);
	void SetSystemTick(ECycleSystemType eType, bool bTick, int32 iIndex = -1);//iIndex -1:push_back

	UFUNCTION(BlueprintPure)
	UPXCycleSystem* GetCycleSystem(ECycleSystemType eType);

	UFUNCTION(BlueprintPure)
	UPxcInputMappingMgr* GetInputMappingMgr();

	UPROPERTY(BlueprintReadWrite)
	bool m_bKeyboardRuntime = true;

private:
	void OnGameModeInitialized(AGameModeBase* pGM);
	void OnPreClientTravel(const FString& sPendingURL, ETravelType eTravelType, bool bIsSeamlessTravel);

	UPROPERTY()
	TMap<ECycleSystemType, UPXCycleSystem*> m_tmapSystems;

	std::list<UPXCycleSystem*> m_lisSystems;
	FDelegateHandle DeleTickHandle;
};
