// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PrivateDefinitions/LevelManagersDef.h"
#include <list>
#include "PxcGameMode.generated.h"

class UVirtualGroupMgr;
class USpecialLevelMgrBase;
class URelativeTimeDilationMgr;
class UGroupCentralTargetMgr;

/**
 * 
 */
UCLASS()
class UEGAME_API APxcGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	APxcGameMode();

	virtual void InitGame(const FString& sMapName, const FString& sOptions, FString& sErrorMessage) override;
	virtual void Tick(float fDeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type eEndPlayReason) override;

	void AddSpecialLevelManager(ESpecialLevelMgrType eType, USpecialLevelMgrBase* pMgr, bool bTick = true);
	void ReleaseSpecialLevelManagers(ESpecialLevelMgrType* pTypes, int32 iTypeNum);
	void SetSpecialLevelManagerTick(ESpecialLevelMgrType eType, bool bTick, int32 iIndex = -1);//iIndex -1:push_back

	UFUNCTION(BlueprintPure)
	UVirtualGroupMgr* GetVirtualGroupMgr();

	UFUNCTION(BlueprintPure)
	USpecialLevelMgrBase* GetSpecialLevelManager(ESpecialLevelMgrType eType);

	UFUNCTION(BlueprintPure)
	URelativeTimeDilationMgr* GetRelativeTimeDilationMgr();

	UFUNCTION(BlueprintPure)
	UGroupCentralTargetMgr* GetGroupCentralTargetMgr();

protected:
	//*≤‚ ‘¡Ÿ ±
	UFUNCTION(BlueprintImplementableEvent, Category = Test)
	void OnHelloWorld(const FString& sText);
	//*/

private:
	UPROPERTY()
	UVirtualGroupMgr* m_pVirtualGroupMgr;

	UPROPERTY()
	TMap<ESpecialLevelMgrType, USpecialLevelMgrBase*> m_tmapSpecialMgrs;

	std::list<USpecialLevelMgrBase*> m_lisSpecialMgrs;
};