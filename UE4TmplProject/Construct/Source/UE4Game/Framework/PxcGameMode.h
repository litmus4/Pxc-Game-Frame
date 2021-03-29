// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PxcGameMode.generated.h"

class UVirtualGroupMgr;
class URelativeTimeDilationMgr;

/**
 * 
 */
UCLASS()
class UE4GAME_API APxcGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	APxcGameMode();

	virtual void InitGame(const FString& sMapName, const FString& sOptions, FString& sErrorMessage) override;
	virtual void Tick(float fDeltaSeconds) override;
	virtual void EndPlay(const EEndPlayReason::Type eEndPlayReason) override;

	UFUNCTION(BlueprintPure)
	UVirtualGroupMgr* GetVirtualGroupMgr();

	UFUNCTION(BlueprintPure)
	URelativeTimeDilationMgr* GetRelativeTimeDilationMgr();

protected:
	//*≤‚ ‘¡Ÿ ±
	UFUNCTION(BlueprintImplementableEvent, Category = Test)
	void OnHelloWorld(const FString& sText);
	//*/

private:
	UPROPERTY()
	UVirtualGroupMgr* m_pVirtualGroupMgr;

	UPROPERTY()
	URelativeTimeDilationMgr* m_pRelTimeDilationMgr;
};