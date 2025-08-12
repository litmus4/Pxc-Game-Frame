// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "EnhancedActionKeyMapping.h"
#include "PrivateDefinitions/MainDef.h"
#include "Structs/InputStructs.h"
#include <list>
#include "PXCycleInstance.generated.h"

class UPXCycleSystem;
class UPxcInputMappingMgr;
class URandomGameplaySystem;
class UPrlInputMappingContext;

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

	UFUNCTION(BlueprintPure)
	URandomGameplaySystem* GetRandomGameplaySystem();

	UPROPERTY(BlueprintReadWrite)
	bool m_bKeyboardRuntime = true;

	UPROPERTY(BlueprintReadOnly)
	bool m_bPIMCConfigLoaded = false;

	UPROPERTY(BlueprintReadOnly)
	UPrlInputMappingContext* m_pPrlIMC = nullptr;

	//如果运行时对PIMC进行了单纯增删，那么请主动调用此函数并传入true来rebuild
	UFUNCTION(BlueprintCallable)
	void BuildQuickActionMap(bool bRebuild = false);

	void SetAndCacheDefaultPIMC(UPrlInputMappingContext* pPrlIMC);
	void ResetDefaultPIMC();

	FORCEINLINE const UInputAction* GetQuickAction(const FName& ActionName) const;
	FORCEINLINE ETriggerEvent GetQuickActionMainBindedEvent(const FName& ActionName) const;

private:
	void OnGameModeInitialized(AGameModeBase* pGM);
	void OnPreClientTravel(const FString& sPendingURL, ETravelType eTravelType, bool bIsSeamlessTravel);

	UPROPERTY()
	TMap<ECycleSystemType, UPXCycleSystem*> m_tmapSystems;

    UPROPERTY()
	TArray<FEnhancedActionKeyMapping> m_tarrDefaultPIMCached;

	UPROPERTY()
	TMap<FName, FPrlInputActionEx> m_tmapQuickActions;

	std::list<UPXCycleSystem*> m_lisSystems;
	FTSTicker::FDelegateHandle DeleTickHandle;
};
