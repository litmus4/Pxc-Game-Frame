// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NetBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API UNetBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking")
	static bool Net_IsListenServer(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	static bool Net_IsActorRoleControlled(AActor* Actor);

	UFUNCTION(BlueprintCallable, Category = "Networking")
	static bool Net_IsActorRoleAuthControlled(AActor* Actor);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static APlayerController* Net_GetLocalPlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static APawn* Net_GetLocalPlayerPawn(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static ACharacter* Net_GetLocalPlayerCharacter(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static APlayerCameraManager* Net_GetLocalPlayerCameraManager(UObject* WorldContextObject);

	static APawn* Net_GetAuthControlledPawnNative(UObject* WorldContextObject, APlayerController** OutPC = nullptr);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static APawn* Net_GetAuthControlledPawn(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static ACharacter* Net_GetAuthControlledCharacter(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static APlayerController* Net_GetAuthControlledController(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static APlayerCameraManager* Net_GetAuthControlledCameraManager(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static int32 Net_GetPlayerCount(UObject* WorldContextObject);

	/* Ä£·ÂOpenLevelµÄServerTravel */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static bool Net_ServerTravel(UObject* WorldContextObject, FName LevelName, bool bAbsolute = true, FString Options = FString(TEXT("")));

private:
	static int32 NetPlayerCount;
};
