// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UEGameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API AUEGameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AUEGameGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = GameMode)
	void OnHelloWorld(const FString& sText);
};
