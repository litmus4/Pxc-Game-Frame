// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/PostRegAllComponentsActor.h"

// Sets default values
APostRegAllComponentsActor::APostRegAllComponentsActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APostRegAllComponentsActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void APostRegAllComponentsActor::PostRegisterAllComponents()
{
	Super::PostRegisterAllComponents();

	OnPostRegisterAllComponents();
}

void APostRegAllComponentsActor::OnConstruction(const FTransform& Transform)
{
	//Super::OnConstruction(Transform);
	OnOnConstruction();
}

void APostRegAllComponentsActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	OnPreInitializeComponents();
}

// Called every frame
void APostRegAllComponentsActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

