// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorStructs.h"
#include "LevelPreparedActor.generated.h"

UCLASS()
class UEGAME_API ALevelPreparedActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALevelPreparedActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPreInitializeComponents();
	virtual void OnPreInitializeComponents_Implementation() {}

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FSubLevelLinkInfo> m_tmapSubLevelLinkInfos;

public:	
	virtual void PreInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
