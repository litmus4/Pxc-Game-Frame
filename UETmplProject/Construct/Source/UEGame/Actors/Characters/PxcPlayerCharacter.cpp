// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
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
	m_vFocus = FVector::ZeroVector;
	m_eMotionState = EMotionState::Idle;
	m_v2Axis.Set(0.0, 0.0);
	m_v2LastAxis.Set(0.0, 0.0);
}

void APxcPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void APxcPlayerCharacter::CheckMachineStateEnd(UAnimInstance* pABP, FName MachineName, EMotionState eMotionState, float fTimeRemaining)
{
	if (!IsValid(pABP)) return;

	int32 iMachineIndex = pABP->GetStateMachineIndex(MachineName);
	if (iMachineIndex != INDEX_NONE)
	{
		const FAnimNode_StateMachine* pMachine = pABP->GetStateMachineInstance(iMachineIndex);
		if (!pMachine) return;

		FName&& CurStateName = pMachine->GetCurrentStateName();
		FName&& MotionName = StaticEnum<EMotionState>()->GetNameByIndex((int32)eMotionState);
		if (CurStateName != NAME_None && CurStateName == MotionName)
		{
			float fCurTimeRemaining = pABP->GetRelevantAnimTimeRemaining(iMachineIndex, pMachine->GetCurrentState());
			if (fCurTimeRemaining <= fTimeRemaining)
				OnMotionStateEnd(eMotionState);
		}
	}
}

void APxcPlayerCharacter::RunLocoMotionEndMoveInput()
{
	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
		AddMovementInput(GetActorForwardVector(), 1.0f);
	else
		AddMovementInput(m_vFocus.GetSafeNormal(), 1.0f);
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

	if (m_v2Axis.IsNearlyZero() && m_v2LastAxis.IsNearlyZero())
		return;

	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
	{
		FRotator&& rCtrlRot = GetControlRotation();
		FVector&& vForward = UKismetMathLibrary::GetForwardVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		FVector&& vRight = UKismetMathLibrary::GetRightVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		if (m_eMotionState == EMotionState::EndMove && !m_bRootMotion)
			m_vFocus = vForward * m_v2LastAxis.Y + vRight * m_v2LastAxis.X;
		else
			m_vFocus =  vForward * m_v2Axis.Y + vRight * m_v2Axis.X;
	}

	if (!m_bRootMotion)
	{
		AddMovementInput(GetActorForwardVector(),
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

	if (m_v2Axis.IsNearlyZero() && m_v2LastAxis.IsNearlyZero())
		return;

	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
	{
		FRotator&& rCtrlRot = GetControlRotation();
		FVector&& vForward = UKismetMathLibrary::GetForwardVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		FVector&& vRight = UKismetMathLibrary::GetRightVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		if (m_eMotionState == EMotionState::EndMove && !m_bRootMotion)
			m_vFocus = vForward * m_v2LastAxis.Y + vRight * m_v2LastAxis.X;
		else
			m_vFocus = vForward * m_v2Axis.Y + vRight * m_v2Axis.X;
	}
	
	if (!m_bRootMotion)
	{
		if (m_eAnimBPType == EAnimBPType::DirectionTurned)
			AddMovementInput(GetActorForwardVector(), 1.0f);
		else
			AddMovementInput(GetActorRightVector(), fValue);
	}
}

void APxcPlayerCharacter::OnMotionStateEnd(EMotionState eMotionState)
{
	switch (eMotionState)
	{
	case EMotionState::StartMove:
		m_eMotionState = EMotionState::Move;
		break;
	case EMotionState::EndMove:
		m_eMotionState = EMotionState::Idle;
		m_vFocus = FVector::ZeroVector;
		if (!m_bRootMotion)
			m_v2LastAxis.Set(0.0, 0.0);
		break;
	}
}