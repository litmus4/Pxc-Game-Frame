// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

APxcPlayerCharacter::APxcPlayerCharacter()
{
	m_pSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	m_pSpringArmComp->bUsePawnControlRotation = true;
	m_pSpringArmComp->SetupAttachment(RootComponent);

	m_pCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	m_pCameraComp->SetupAttachment(m_pSpringArmComp);
}

void APxcPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void APxcPlayerCharacter::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

}

void APxcPlayerCharacter::SetupPlayerInputComponent(UInputComponent* pPlayerInputComponent)
{
	Super::SetupPlayerInputComponent(pPlayerInputComponent);

	pPlayerInputComponent->BindAxis("MoveForward", this, &APxcPlayerCharacter::OnMoveForward);
	pPlayerInputComponent->BindAxis("MoveRight", this, &APxcPlayerCharacter::OnMoveRight);

	pPlayerInputComponent->BindAxis("PitchDown", this, &APxcPlayerCharacter::AddControllerPitchInput);
	pPlayerInputComponent->BindAxis("YawRight", this, &APxcPlayerCharacter::AddControllerYawInput);
}

void APxcPlayerCharacter::OnMoveForward(float fValue)
{
	//
}

void APxcPlayerCharacter::OnMoveRight(float fValue)
{
	//
}