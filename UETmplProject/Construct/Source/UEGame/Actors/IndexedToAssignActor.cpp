// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/IndexedToAssignActor.h"

// Sets default values
AIndexedToAssignActor::AIndexedToAssignActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

void AIndexedToAssignActor::RunAssignName_Implementation(FName Name)
{
	m_AssignedName = Name;
}

void AIndexedToAssignActor::RunAssignNumber_Implementation(int32 iNumber)
{
	m_iAssignedNumber = iNumber;
}

void AIndexedToAssignActor::RunAssignData(void* pData)
{
	m_pAssignedData = pData;
}

// Called when the game starts or when spawned
void AIndexedToAssignActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AIndexedToAssignActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

