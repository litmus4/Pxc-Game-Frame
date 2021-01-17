// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Framework/Structs/UDTRowStructs.h"
#include "PxcStaticResourceSystem.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API UPxcStaticResourceSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

protected:
	TMap<FGameplayTag, FUTRGExContToInst> m_tmapUDTGEExtentionContToInst;
};
