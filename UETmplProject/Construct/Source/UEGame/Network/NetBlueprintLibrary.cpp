// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "EngineUtils.h"
#include "NetTypesWrapper.h"
#include "Actors/Characters/PxcPlayerCharacter.h"

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

APawn* UNetBlueprintLibrary::Net_GetLocalPlayerPawnCli(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	if (World->GetNetMode() != NM_Client)
	{
		for (FConstPlayerControllerIterator Iter1 = World->GetPlayerControllerIterator(); Iter1; Iter1++)
		{
			APlayerController* Controller = Iter1->Get();
			if (Controller && Controller->IsLocalController())
				return Controller->GetPawnOrSpectator();
		}
	}

	for (TActorIterator<APxcPlayerCharacter> Iter2(World); Iter2; ++Iter2)
	{
		if (*Iter2 && (*Iter2)->GetLocalRole() == ROLE_AutonomousProxy)
			return *Iter2;
	}
	return nullptr;
}

ACharacter* UNetBlueprintLibrary::Net_GetLocalPlayerCharacterCli(UObject* WorldContextObject)
{
	APawn* Pawn = Net_GetLocalPlayerPawnCli(WorldContextObject);
	return Pawn ? Cast<ACharacter>(Pawn) : nullptr;
}

APawn* UNetBlueprintLibrary::Net_GetAuthControlledPawnNative(UObject* WorldContextObject, APlayerController** OutPC)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	for (TActorIterator<APxcPlayerCharacter> Iter(World); Iter; ++Iter)
	{
		APawn* Pawn = *Iter;
		if (Pawn && ((Pawn->GetLocalRole() == ROLE_Authority && Pawn->GetRemoteRole() == ROLE_SimulatedProxy) ||
			(Pawn->GetLocalRole() == ROLE_SimulatedProxy && Pawn->GetRemoteRole() == ROLE_Authority)))
		{
			if (OutPC) *OutPC = Pawn->GetController<APlayerController>();
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

ACharacter* UNetBlueprintLibrary::Net_GetRandomPlayerCharacter(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return nullptr;

	TArray<APawn*> Pawns;
	int32 Num = 0;
	for (FConstPlayerControllerIterator Iter = World->GetPlayerControllerIterator(); Iter; Iter++)
	{
		if (!Iter->Get()) continue;
		Pawns.Add(Iter->Get()->GetPawn());
		Num++;
	}
	return Cast<ACharacter>(Pawns[FMath::RandHelper(Num)]);
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

int32 UNetBlueprintLibrary::Net_GetPlayerCountCli(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return 0;

	int32 Ret = 0;
	for (TActorIterator<APxcPlayerCharacter> Iter(World); Iter; ++Iter)
	{
		if (*Iter)
			Ret++;
	}
	return Ret;
}


bool UNetBlueprintLibrary::Net_ForEachPlayerNative(UObject* WorldContextObject, const FNetEachPlayerDelegate& OnEach)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World) return false;

	for (TActorIterator<APxcPlayerCharacter> Iter(World); Iter; ++Iter)
	{
		if (!(*Iter)) continue;
		if (OnEach.Execute(*Iter))
			return true;
	}
	return false;
}

void UNetBlueprintLibrary::Net_ForEachPlayer(UObject* WorldContextObject, FNetDynOnEachPlayerDelegate OnEach)
{
	Net_ForEachPlayerNative(WorldContextObject,
		FNetEachPlayerDelegate::CreateLambda([&OnEach](APxcPlayerCharacter* Player)->bool {
			OnEach.ExecuteIfBound(Player);
			return false;
		}));
}

bool UNetBlueprintLibrary::Net_IsAnyPlayer(UObject* WorldContextObject, AActor* Actor)
{
	return Net_ForEachPlayerNative(WorldContextObject,
		FNetEachPlayerDelegate::CreateLambda([Actor](APxcPlayerCharacter* Player)->bool {
			return (Player == Actor);
		}));
}

void UNetBlueprintLibrary::Net_BindAllPlayersUniversalEvent(UObject* WorldContextObject, EPlayerUniEventType Type, FPlayerUniversalOneDelegate OnCall)
{
	Net_ForEachPlayerNative(WorldContextObject,
		FNetEachPlayerDelegate::CreateLambda([Type, &OnCall](APxcPlayerCharacter* Player)->bool {
			Player->BindUniversalEventByType(Type, OnCall);
			return false;
		}));
}

void UNetBlueprintLibrary::Net_UnbindAllPlayersAllUniversalEvents(UObject* WorldContextObject, EPlayerUniEventType Type)
{
	Net_ForEachPlayerNative(WorldContextObject,
		FNetEachPlayerDelegate::CreateLambda([Type](APxcPlayerCharacter* Player)->bool {
			Player->UnbindAllUniversalEventsFromType(Type);
			return false;
		}));
}

bool UNetBlueprintLibrary::Net_IsComponentOverlappingAnyPlayer(UObject* WorldContextObject, UPrimitiveComponent* Component)
{
	if (!IsValid(Component)) return false;

	return Net_ForEachPlayerNative(WorldContextObject,
		FNetEachPlayerDelegate::CreateLambda([Component](APxcPlayerCharacter* Player)->bool {
			return Component->IsOverlappingActor(Player);
		}));
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

FSharedSignature UNetBlueprintLibrary::Net_WrapIntegerParam(int32 iData)
{
	FNetIntegerParamWrapper Wrapper(iData);
	return FSharedSignature(Wrapper);
}

FSharedSignature UNetBlueprintLibrary::Net_WrapBoolParam(bool bData)
{
	FNetBoolParamWrapper Wrapper(bData);
	return FSharedSignature(Wrapper);
}

FSharedSignature UNetBlueprintLibrary::Net_WrapObjectParam(UObject* pObject)
{
	FNetObjectParamWrapper Wrapper(pObject);
	return FSharedSignature(Wrapper);
}

FSharedSignature UNetBlueprintLibrary::Net_WrapClassParam(TSubclassOf<UObject> cClass)
{
	FNetClassParamWrapper Wrapper(cClass);
	return FSharedSignature(Wrapper);
}

FSharedSignature UNetBlueprintLibrary::Net_WrapGameplayTagParam(FGameplayTag Tag)
{
	FNetGameplayTagParamWrapper Wrapper(Tag);
	return FSharedSignature(Wrapper);
}

FSharedSignature UNetBlueprintLibrary::Net_WrapTreeParam(const TArray<FSharedSignature>& tarrChildren)
{
	FNetTreeParamWrapper Wrapper(tarrChildren);
	return FSharedSignature(Wrapper);
}

int32 UNetBlueprintLibrary::Net_UnwrapToInteger(FSharedSignature ParamSig)
{
	FNetIntegerParamWrapper* pWrapper = ParamSig.GetDerivedPtr<FNetIntegerParamWrapper>();
	return pWrapper->iData;
}

bool UNetBlueprintLibrary::Net_UnwrapToBool(FSharedSignature ParamSig)
{
	FNetBoolParamWrapper* pWrapper = ParamSig.GetDerivedPtr<FNetBoolParamWrapper>();
	return pWrapper->bData;
}

UObject* UNetBlueprintLibrary::Net_UnwrapToObject(FSharedSignature ParamSig)
{
	FNetObjectParamWrapper* pWrapper = ParamSig.GetDerivedPtr<FNetObjectParamWrapper>();
	return pWrapper->pObject;
}

TSubclassOf<UObject> UNetBlueprintLibrary::Net_UnwrapToClass(FSharedSignature ParamSig)
{
	FNetClassParamWrapper* pWrapper = ParamSig.GetDerivedPtr<FNetClassParamWrapper>();
	return pWrapper->cClass;
}

FGameplayTag UNetBlueprintLibrary::Net_UnwrapToGameplayTag(FSharedSignature ParamSig)
{
	FNetGameplayTagParamWrapper* pWrapper = ParamSig.GetDerivedPtr<FNetGameplayTagParamWrapper>();
	return pWrapper->Tag;
}

void UNetBlueprintLibrary::Net_UnwrapToTree(FSharedSignature ParamSig, TArray<FSharedSignature>& tarrOut)
{
	FNetTreeParamWrapper* pWrapper = ParamSig.GetDerivedPtr<FNetTreeParamWrapper>();
	tarrOut = pWrapper->tarrChildren;
}