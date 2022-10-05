// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
#include "Network/NetTypesCommon.h"
#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Framework/Structs/SignatureStructs.h"
#include "PrivateDefinitions/CharacterDef.h"
#include "NetBlueprintLibrary.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API UNetBlueprintLibrary : public UBlueprintFunctionLibrary
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

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static APawn* Net_GetLocalPlayerPawnCli(UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static ACharacter* Net_GetLocalPlayerCharacterCli(UObject* WorldContextObject);

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

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static int32 Net_GetPlayerCountCli(UObject* WorldContextObject);

	static bool Net_ForEachPlayerNative(UObject* WorldContextObject, const FNetEachPlayerDelegate& OnEach);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static void Net_ForEachPlayer(UObject* WorldContextObject, FNetDynOnEachPlayerDelegate OnEach);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static bool Net_IsAnyPlayer(UObject* WorldContextObject, AActor* Actor);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static void Net_BindAllPlayersUniversalEvent(UObject* WorldContextObject, EPlayerUniEventType Type, FPlayerUniversalOneDelegate OnCall);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static void Net_UnbindAllPlayersAllUniversalEvents(UObject* WorldContextObject, EPlayerUniEventType Type);

	UFUNCTION(BlueprintPure, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static bool Net_IsComponentOverlappingAnyPlayer(UObject* WorldContextObject, UPrimitiveComponent* Component);

	/* Ä£·ÂOpenLevelµÄServerTravel */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Networking|Game")
	static bool Net_ServerTravel(UObject* WorldContextObject, FName LevelName, bool bAbsolute = true, FString Options = FString(TEXT("")));

	//Net Param Wrapping
	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static FSharedSignature Net_WrapIntegerParam(int32 iData);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static FSharedSignature Net_WrapBoolParam(bool bData);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static FSharedSignature Net_WrapObjectParam(UObject* pObject);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static FSharedSignature Net_WrapClassParam(TSubclassOf<UObject> cClass);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static FSharedSignature Net_WrapGameplayTagParam(FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static FSharedSignature Net_WrapTreeParam(const TArray<FSharedSignature>& tarrChildren);

	//Net Param Unwrapping
	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static int32 Net_UnwrapToInteger(FSharedSignature ParamSig);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static bool Net_UnwrapToBool(FSharedSignature ParamSig);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static UObject* Net_UnwrapToObject(FSharedSignature ParamSig);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static TSubclassOf<UObject> Net_UnwrapToClass(FSharedSignature ParamSig);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static FGameplayTag Net_UnwrapToGameplayTag(FSharedSignature ParamSig);

	UFUNCTION(BlueprintPure, Category = "Networking|Other")
	static void Net_UnwrapToTree(FSharedSignature ParamSig, TArray<FSharedSignature>& tarrOut);

private:
	static int32 NetPlayerCount;
};
