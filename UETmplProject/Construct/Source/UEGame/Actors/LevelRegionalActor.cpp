// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LevelRegionalActor.h"

// Sets default values
ALevelRegionalActor::ALevelRegionalActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelRegionalActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelRegionalActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

