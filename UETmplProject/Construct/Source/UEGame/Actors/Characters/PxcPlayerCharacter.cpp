// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"

APxcPlayerCharacter::APxcPlayerCharacter()
{
	m_pSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	m_pSpringArmComp->bUsePawnControlRotation = true;
	m_pSpringArmComp->SetupAttachment(RootComponent);

	m_pCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	m_pCameraComp->SetupAttachment(m_pSpringArmComp);

	m_bRootMotion = false;
	m_eAnimBPType = EAnimBPType::DirectionTurned;
	m_vTurn.Set(0.0, 0.0, 0.0);
	m_eMotionState = EMotionState::Idle;
	m_v2Axis.Set(0.0, 0.0);
	m_v2LastAxis.Set(0.0, 0.0);
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
	FVector2D v2LastAxis = m_v2Axis;
	m_v2Axis.Y = fValue;
	if (!m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Idle ||
		m_eMotionState == EMotionState::EndMove))
	{
		m_eMotionState = EMotionState::StartMove;
		if (!m_bRootMotion)
			m_v2LastAxis.Set(0.0, 0.0);
	}
	else if (m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Move ||
		m_eMotionState == EMotionState::StartMove))
	{
		m_eMotionState = EMotionState::EndMove;
		if (!m_bRootMotion)
			m_v2LastAxis = v2LastAxis;
	}

	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
	{
		FRotator&& rCtrlRot = GetControlRotation();
		FVector&& vForward = UKismetMathLibrary::GetForwardVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		FVector&& vRight = UKismetMathLibrary::GetRightVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		if (m_eMotionState == EMotionState::EndMove && !m_bRootMotion)
			m_vTurn = vForward * m_v2LastAxis.Y + vRight * m_v2LastAxis.X;
		else
			m_vTurn =  vForward * m_v2Axis.Y + vRight * m_v2Axis.X;
	}

	if (!m_bRootMotion)
	{
		AddMovementInput(GetActorForwardVector() *
			(m_eAnimBPType == EAnimBPType::DirectionTurned ? 1.0f : fValue));
	}
}

void APxcPlayerCharacter::OnMoveRight(float fValue)
{
	FVector2D v2LastAxis = m_v2Axis;
	m_v2Axis.Y = fValue;
	if (!m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Idle ||
		m_eMotionState == EMotionState::EndMove))
	{
		m_eMotionState = EMotionState::StartMove;
		if (!m_bRootMotion)
			m_v2LastAxis.Set(0.0, 0.0);
	}
	else if (m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Move ||
		m_eMotionState == EMotionState::StartMove))
	{
		m_eMotionState = EMotionState::EndMove;
		if (!m_bRootMotion)
			m_v2LastAxis = v2LastAxis;
	}

	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
	{
		FRotator&& rCtrlRot = GetControlRotation();
		FVector&& vForward = UKismetMathLibrary::GetForwardVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		FVector&& vRight = UKismetMathLibrary::GetRightVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		if (m_eMotionState == EMotionState::EndMove && !m_bRootMotion)
			m_vTurn = vForward * m_v2LastAxis.Y + vRight * m_v2LastAxis.X;
		else
			m_vTurn = vForward * m_v2Axis.Y + vRight * m_v2Axis.X;
	}
	
	if (!m_bRootMotion)
	{
		if (m_eAnimBPType == EAnimBPType::DirectionTurned)
			AddMovementInput(GetActorForwardVector() * 1.0f);
		else
			AddMovementInput(GetActorRightVector() * fValue);
	}
}