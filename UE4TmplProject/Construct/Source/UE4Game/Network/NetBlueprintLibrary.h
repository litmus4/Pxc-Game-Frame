// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Network/NetTypesCommon.h"
#include "PrivateDefinitions/ScatteredDef.h"
#include "NetBlueprintLibrary.generated.h"

//TODOJK 定义在Player类
DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerUniversalOneDelegate, /*class APxcCharacterPlayer**/ACharacter*, Player);

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
	static ACharacter* Net_GetRandomPlayerCharacter(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static int32 Net_GetPlayerCount(UObject* WorldContextObject);

	static bool Net_ForEachPlayerNative(UObject* WorldContextObject, const FNetEachPlayerDelegate& OnEach);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static void Net_ForEachPlayer(UObject* WorldContextObject, FPlayerUniversalOneDelegate OnEach);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static void Net_BindAllPlayersUniversalEvent(UObject* WorldContextObject, EPlayerUniEventType Type, FPlayerUniversalOneDelegate OnCall);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static void Net_UnbindAllPlayersAllUniversalEvents(UObject* WorldContextObject, EPlayerUniEventType Type);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static bool Net_IsComponentOverlappingAnyPlayer(UObject* WorldContextObject, UPrimitiveComponent* Component);

	/* 模仿OpenLevel的ServerTravel */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static bool Net_ServerTravel(UObject* WorldContextObject, FName LevelName, bool bAbsolute = true, FString Options = FString(TEXT("")));

private:
	static int32 NetPlayerCount;
};
