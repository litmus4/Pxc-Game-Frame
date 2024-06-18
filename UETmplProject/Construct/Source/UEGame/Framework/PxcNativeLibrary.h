// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <functional>
#include "PxcNativeLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UPxcNativeLibrary : public UObject
{
	GENERATED_BODY()
	
};

struct FPxcNativeLibrary
{
	FPxcNativeLibrary() = delete;

	static void GetAllActorsInSubLevel(UWorld* pWorld, ULevelStreaming* pLevelStreaming, TSubclassOf<AActor> Class, TArray<AActor*>& tarrOutActors,
		std::function<bool(AActor*)> fnFilter = [](AActor* pActor) { return true; });
};