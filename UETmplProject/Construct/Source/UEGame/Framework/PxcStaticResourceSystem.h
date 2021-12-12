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
class UEGAME_API UPxcStaticResourceSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UPxcStaticResourceSystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	const FUTRGExContToInst* GetUTRGEExtentionContToInst(const FGameplayTag& ExtentionTag);

protected:
	static UPxcStaticResourceSystem* s_pInst;

	TMap<FGameplayTag, FUTRGExContToInst> m_tmapUDTGEExtentionContToInst;
};
