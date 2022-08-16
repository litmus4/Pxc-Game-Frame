// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/PxcGraphCharacter.h"

// Sets default values
APxcGraphCharacter::APxcGraphCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APxcGraphCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APxcGraphCharacter::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

}

// Called to bind functionality to input
//void APxcGraphCharacter::SetupPlayerInputComponent(UInputComponent* pPlayerInputComponent)
//{
//	Super::SetupPlayerInputComponent(pPlayerInputComponent);
//
//}