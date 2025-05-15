// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PostRegAllComponentsActor.generated.h"

UCLASS()
class UEGAME_API APostRegAllComponentsActor : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	APostRegAllComponentsActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPostRegisterAllComponents();
	virtual void OnPostRegisterAllComponents_Implementation() {}

	UFUNCTION(BlueprintNativeEvent)
	void OnOnConstruction();
	virtual void OnOnConstruction_Implementation() {}

	UFUNCTION(BlueprintNativeEvent)
	void OnPreInitializeComponents();
	virtual void OnPreInitializeComponents_Implementation() {}

public:
	virtual void PostRegisterAllComponents() override;
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void PreInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

};