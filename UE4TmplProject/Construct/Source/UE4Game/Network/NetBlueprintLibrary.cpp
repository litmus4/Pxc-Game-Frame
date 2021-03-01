// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetBlueprintLibrary.h"
#include "GameFramework/Character.h"

int32 UNetBlueprintLibrary::NetPlayerCount = 0;

bool UNetBlueprintLibrary::Net_IsListenServer(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return (World ? World->GetNetMode() == ENetMode::NM_ListenServer : false);
}

bool UNetBlueprintLibrary::Net_IsActorRoleControlled(AActor* Actor)
{
	if (Actor)
	{
		ENetRole LocalRole = Actor->GetLocalRole(), RemoteRole = Actor->GetRemoteRole();
		return ((LocalRole == ROLE_Authority && RemoteRole == ROLE_SimulatedProxy) ||
			(LocalRole == ROLE_AutonomousProxy && RemoteRole == ROLE_Authority));
	}
	return false;
}

bool UNetBlueprintLibrary::Net_IsActorRoleAuthControlled(AActor* Actor)
{
	if (Actor)
	{
		ENetRole LocalRole = Actor->GetLocalRole(), RemoteRole = Actor->GetRemoteRole();
		return ((LocalRole == ROLE_Authority && RemoteRole == ROLE_SimulatedProxy) ||
			(LocalRole == ROLE_SimulatedProxy && RemoteRole == ROLE_Authority));
	}
	return false;
}

APlayerController* UNetBlueprintLibrary::Net_GetLocalPlayerController(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	for (FConstPlayerControllerIterator Iter = World->GetPlayerControllerIterator(); Iter; Iter++)
	{
		APlayerController* Controller = Iter->Get();
		if (Controller && Controller->IsLocalController())
			return Controller;
	}
	return nullptr;
}

APawn* UNetBlueprintLibrary::Net_GetLocalPlayerPawn(UObject* WorldContextObject)
{
	APlayerController* Controller = Net_GetLocalPlayerController(WorldContextObject);
	return Controller ? Controller->GetPawnOrSpectator() : nullptr;
}

ACharacter* UNetBlueprintLibrary::Net_GetLocalPlayerCharacter(UObject* WorldContextObject)
{
	APlayerController* Controller = Net_GetLocalPlayerController(WorldContextObject);
	return Controller ? Cast<ACharacter>(Controller->GetPawn()) : nullptr;
}

APlayerCameraManager* UNetBlueprintLibrary::Net_GetLocalPlayerCameraManager(UObject* WorldContextObject)
{
	APlayerController* Controller = Net_GetLocalPlayerController(WorldContextObject);
	return Controller ? Controller->PlayerCameraManager : nullptr;
}

APawn* UNetBlueprintLibrary::Net_GetAuthControlledPawnNative(UObject* WorldContextObject, APlayerController** OutPC)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	for (FConstPlayerControllerIterator Iter = World->GetPlayerControllerIterator(); Iter; Iter++)
	{
		if (!Iter->Get()) continue;
		APawn* Pawn = Iter->Get()->GetPawn();
		if (Pawn && ((Pawn->GetLocalRole() == ROLE_Authority && Pawn->GetRemoteRole() == ROLE_SimulatedProxy) ||
			(Pawn->GetLocalRole() == ROLE_SimulatedProxy && Pawn->GetRemoteRole() == ROLE_Authority)))
		{
			if (OutPC) *OutPC = Iter->Get();
			return Pawn;
		}
	}
	return nullptr;
}

APawn* UNetBlueprintLibrary::Net_GetAuthControlledPawn(UObject* WorldContextObject)
{
	return Net_GetAuthControlledPawnNative(WorldContextObject);
}

ACharacter* UNetBlueprintLibrary::Net_GetAuthControlledCharacter(UObject* WorldContextObject)
{
	return Cast<ACharacter>(Net_GetAuthControlledPawnNative(WorldContextObject));
}

APlayerController* UNetBlueprintLibrary::Net_GetAuthControlledController(UObject* WorldContextObject)
{
	APlayerController* PC = nullptr;
	Net_GetAuthControlledPawnNative(WorldContextObject, &PC);
	return PC;
}

APlayerCameraManager* UNetBlueprintLibrary::Net_GetAuthControlledCameraManager(UObject* WorldContextObject)
{
	APlayerController* PC = nullptr;
	Net_GetAuthControlledPawnNative(WorldContextObject, &PC);
	return PC ? PC->PlayerCameraManager : nullptr;
}

int32 UNetBlueprintLibrary::Net_GetPlayerCount(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World || World->GetNetMode() != NM_ListenServer) return 0;

	if (NetPlayerCount == 0)//TODOJK_Net 所有玩家断开时重置
	{
		for (FConstPlayerControllerIterator Iter = World->GetPlayerControllerIterator(); Iter; Iter++)
		{
			if (Iter->Get())
				NetPlayerCount++;
		}
	}
	return NetPlayerCount;
}

bool UNetBlueprintLibrary::Net_ServerTravel(UObject* WorldContextObject, FName LevelName, bool bAbsolute, FString Options)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;

	const ETravelType TravelType = (bAbsolute ? TRAVEL_Absolute : TRAVEL_Relative);
	FString Cmd = LevelName.ToString();
	if (Options.Len() > 0)
		Cmd += FString(TEXT("?")) + Options;

	return World->ServerTravel(Cmd, bAbsolute);
}