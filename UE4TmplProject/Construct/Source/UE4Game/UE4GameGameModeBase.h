// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "UE4GameGameModeBase.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API AUE4GameGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AUE4GameGameModeBase();

	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage);

	UFUNCTION(BlueprintImplementableEvent, Category=GameMode)
	void OnHelloWorld(FString& sText);
};
