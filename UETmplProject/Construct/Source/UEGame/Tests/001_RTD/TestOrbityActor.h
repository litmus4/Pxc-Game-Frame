// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Utilities/UtilityStructs.h"
#include "TestOrbityActor.generated.h"

UCLASS()
class UEGAME_API ATestOrbityActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestOrbityActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float fDeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void Orbit(float fTime, AActor* pBaseActor, const FTransform& transStand, FSimpleDynamicDelegate DeleEnded);

protected:
	FTransform m_transStand;
	FSimpleDynamicDelegate m_DeleEnded;

	double m_dCamAngle;
	double m_dCamR;
	double m_dCamAngleOffset;
	double m_dCamAngleSpeed;
};
