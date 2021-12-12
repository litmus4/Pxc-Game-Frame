// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestTriggeredActor.generated.h"

class UStaticMeshComponent;
class UBoxComponent;

UCLASS()
class UEGAME_API ATestTriggeredActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATestTriggeredActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UStaticMeshComponent* m_pMeshComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UBoxComponent* m_pCppBoxComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UBoxComponent* m_pBpBoxComp;

	UFUNCTION()
	void OnBoxOverlapped(UPrimitiveComponent* pOverlappedComp, AActor* pOtherActor, UPrimitiveComponent* pOtherComp,
		int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
