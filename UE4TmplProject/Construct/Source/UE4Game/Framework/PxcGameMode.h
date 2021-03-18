// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "PxcGameMode.generated.h"

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

	UFUNCTION(BlueprintPure)
	URelativeTimeDilationMgr* GetRelativeTimeDilationMgr();

protected:
	//*≤‚ ‘¡Ÿ ±
	UFUNCTION(BlueprintImplementableEvent, Category = Test)
	void OnHelloWorld(const FString& sText);
	//*/

private:
	UPROPERTY()
	URelativeTimeDilationMgr* m_pRelTimeDilationMgr;
};