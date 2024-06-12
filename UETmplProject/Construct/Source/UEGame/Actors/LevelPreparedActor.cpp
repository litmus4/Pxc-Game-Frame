// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LevelPreparedActor.h"

// Sets default values
ALevelPreparedActor::ALevelPreparedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelPreparedActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ALevelPreparedActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	OnPreInitializeComponents();
}

// Called every frame
void ALevelPreparedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

