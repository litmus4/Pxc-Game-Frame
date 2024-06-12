// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "IndexedToAssignActor.generated.h"

UCLASS()
class UEGAME_API AIndexedToAssignActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AIndexedToAssignActor();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RunAssignName(FName Name);
	virtual void RunAssignName_Implementation(FName Name);

	UPROPERTY(BlueprintReadWrite)
	FName m_AssignedName;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void RunAssignNumber(int32 Number);
	virtual void RunAssignNumber_Implementation(int32 iNumber);

	UPROPERTY(BlueprintReadWrite)
	int32 m_iAssignedNumber = -1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 m_iIndexInWorld = -1;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<AActor> m_pHelpingActor;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
