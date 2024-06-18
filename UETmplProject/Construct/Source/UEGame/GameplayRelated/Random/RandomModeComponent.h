// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PrivateDefinitions/MainDef.h"
#include "Framework/Structs/SystemStructs.h"
#include "Actors/IndexedToAssignActor.h"
#include "RandomModeComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEGAME_API URandomModeComponent : public UActorComponent
{
	GENERATED_BODY()
	friend class URandomGameplaySystem;

public:	
	// Sets default values for this component's properties
	URandomModeComponent();

	UFUNCTION(BlueprintCallable)
	void SetRandomModeType(ERandomModeType Type);

	UFUNCTION(BlueprintCallable)
	void RunRandomTeleport(class AIndexedToAssignActor* pActor, int32 iSelectedIndex = -1);

	UFUNCTION(BlueprintCallable)
	void RemoveFromGameMode();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ERandomModeType m_eModeType;

	//Mode: Zhang Gun Neural

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 m_iZGNMinWide = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 m_iZGNMaxWide = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 m_iZGNLayerNum;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<ERandomZGNRoomType, int32> m_tmapZGNTypedRoomMaxNums;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 m_iZGNNextRoomMaxLink = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<ERandomZGNRoomType, FRandomSubLevelList> m_tmapZGNStaticTypedRooms;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AIndexedToAssignActor> m_cTeleportActor;

	UPROPERTY()
	int32 m_iCurrLayerIndex;

	UPROPERTY()
	int32 m_iCurrNodeIndex;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetStreamingLevelInstance(ULevelStreaming* pLSObj);
};
