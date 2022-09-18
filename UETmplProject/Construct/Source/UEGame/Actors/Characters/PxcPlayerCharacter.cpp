// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "PxcUtil/DateTime.h"

APxcPlayerCharacter::APxcPlayerCharacter()
{
	m_pSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	m_pSpringArmComp->bUsePawnControlRotation = true;
	m_pSpringArmComp->SetupAttachment(RootComponent);

	m_pCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	m_pCameraComp->SetupAttachment(m_pSpringArmComp);

	m_bRootMotion = false;
	m_eAnimBPType = EAnimBPType::DirectionTurned;
	m_pStartMoveCurve = nullptr;
	m_pEndMoveCurve = nullptr;
	m_fFootDownLength = 100.0f;

	m_vFocus = FVector::ZeroVector;
	m_eMotionState = EMotionState::Idle;
	m_fStartMoveMaxTime = 0.0f;
	m_fEndMoveMaxTime = 0.0f;
	m_eFootType = EEndMoveFootType::Any;

	m_v2Axis.Set(0.0, 0.0);
	m_v2LastAxis.Set(0.0, 0.0);
	m_fStartMoveTime = -1.0f;
	m_fEndMoveTime = -1.0f;
	m_fStartMoveStamp = 0.0f;
	m_fEndMoveStamp = 0.0f;
	m_bEndMoveStarted = false;
}

void APxcPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	float fTempMin = 0.0f;
	if (IsValid(m_pStartMoveCurve))
		m_pStartMoveCurve->FloatCurve.GetTimeRange(fTempMin, m_fStartMoveMaxTime);
	if (IsValid(m_pEndMoveCurve))
		m_pEndMoveCurve->FloatCurve.GetTimeRange(fTempMin, m_fEndMoveMaxTime);
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
		FName&& MotionName = FName(StaticEnum<EMotionState>()->GetNameByIndex((int32)eMotionState).ToString().RightChop(14));
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
	float fValue = 1.0f;
	if (IsValid(m_pEndMoveCurve))
	{
		if (m_fEndMoveTime >= -0.5f && m_fEndMoveTime <= m_fEndMoveMaxTime)
		{
			float fCurTime = PxcUtil::ExactTime::GetFloatTime();
			m_fEndMoveTime = fCurTime - m_fEndMoveStamp;
			fValue = FMath::Clamp(m_pEndMoveCurve->GetFloatValue(FMath::Min(m_fEndMoveTime, m_fEndMoveMaxTime)), 0.0f, 0.1f);
		}
		else if (m_fEndMoveTime < -0.5f)
		{
			m_fEndMoveTime = 0.0f;
			m_fEndMoveStamp = PxcUtil::ExactTime::GetFloatTime();
			fValue = FMath::Clamp(m_pEndMoveCurve->GetFloatValue(FMath::Min(m_fEndMoveTime, m_fEndMoveMaxTime)), 0.0f, 0.1f);
		}
		else
		{
			ResetLocoMotionEndMoveTime();
			fValue = 0.0f;
		}
	}

	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
		AddMovementInput(GetActorForwardVector(), fValue);
	else
		AddMovementInput(m_vFocus.GetSafeNormal(), fValue);
}

void APxcPlayerCharacter::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

	if (m_bEndMoveStarted)
	{
		OnEndMoveStarted();
		m_bEndMoveStarted = false;
	}
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
	m_v2Axis.X = fValue;
	if (!m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Idle ||
		m_eMotionState == EMotionState::EndMove))
	{
		m_eMotionState = EMotionState::StartMove;
		if (!m_bRootMotion)
		{
			m_v2LastAxis.Set(0.0, 0.0);
			if (IsValid(m_pStartMoveCurve))
			{
				m_fStartMoveTime = 0.0f;
				m_fStartMoveStamp = PxcUtil::ExactTime::GetFloatTime();
			}
		}
	}
	else if (m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Move ||
		m_eMotionState == EMotionState::StartMove))
	{
		m_eMotionState = EMotionState::EndMove;
		if (!m_bRootMotion)
			m_v2LastAxis = v2LastAxis;
		m_bEndMoveStarted = true;
	}

	if (m_v2Axis.IsNearlyZero() && m_v2LastAxis.IsNearlyZero())
		return;

	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
	{
		FRotator&& rCtrlRot = GetControlRotation();
		FVector&& vForward = UKismetMathLibrary::GetForwardVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		FVector&& vRight = UKismetMathLibrary::GetRightVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		if (m_eMotionState == EMotionState::EndMove && !m_bRootMotion)
			m_vFocus = vForward * m_v2LastAxis.X + vRight * m_v2LastAxis.Y;
		else
			m_vFocus =  vForward * m_v2Axis.X + vRight * m_v2Axis.Y;
	}

	if (!m_bRootMotion)
	{
		if (IsValid(m_pStartMoveCurve))
		{
			if (m_fStartMoveTime >= -0.5f && m_fStartMoveTime <= m_fStartMoveMaxTime)
			{
				float fCurTime = PxcUtil::ExactTime::GetFloatTime();
				m_fStartMoveTime = fCurTime - m_fStartMoveStamp;
				if (!m_v2Axis.IsNearlyZero())
					fValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(m_fStartMoveTime, m_fStartMoveMaxTime)), 0.0f, 0.1f);
			}
			else
				ResetLocoMotionStartMoveTime();
		}
		AddMovementInput(GetActorForwardVector(),
			(m_eAnimBPType == EAnimBPType::DirectionTurned ? FMath::Abs(fValue) : fValue));
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
		{
			m_v2LastAxis.Set(0.0, 0.0);
			if (IsValid(m_pStartMoveCurve))
			{
				m_fStartMoveTime = 0.0f;
				m_fStartMoveStamp = PxcUtil::ExactTime::GetFloatTime();
			}
		}
	}
	else if (m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Move ||
		m_eMotionState == EMotionState::StartMove))
	{
		m_eMotionState = EMotionState::EndMove;
		if (!m_bRootMotion)
			m_v2LastAxis = v2LastAxis;
		m_bEndMoveStarted = true;
	}

	if (m_v2Axis.IsNearlyZero() && m_v2LastAxis.IsNearlyZero())
		return;

	if (m_eAnimBPType == EAnimBPType::DirectionTurned)
	{
		FRotator&& rCtrlRot = GetControlRotation();
		FVector&& vForward = UKismetMathLibrary::GetForwardVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		FVector&& vRight = UKismetMathLibrary::GetRightVector(FRotator(0.0, rCtrlRot.Yaw, 0.0));
		if (m_eMotionState == EMotionState::EndMove && !m_bRootMotion)
			m_vFocus = vForward * m_v2LastAxis.X + vRight * m_v2LastAxis.Y;
		else
			m_vFocus = vForward * m_v2Axis.X + vRight * m_v2Axis.Y;
	}
	
	if (!m_bRootMotion)
	{
		if (IsValid(m_pStartMoveCurve))
		{
			if (m_fStartMoveTime >= -0.5f && m_fStartMoveTime <= m_fStartMoveMaxTime)
			{
				float fCurTime = PxcUtil::ExactTime::GetFloatTime();
				m_fStartMoveTime = fCurTime - m_fStartMoveStamp;
				if (!m_v2Axis.IsNearlyZero())
					fValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(m_fStartMoveTime, m_fStartMoveMaxTime)), 0.0f, 0.1f);
			}
			else
				ResetLocoMotionStartMoveTime();
		}
		if (m_eAnimBPType == EAnimBPType::DirectionTurned)
			AddMovementInput(GetActorForwardVector(), FMath::Abs(fValue));
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
		m_eFootType = EEndMoveFootType::Any;
		if (!m_bRootMotion)
		{
			m_v2LastAxis.Set(0.0, 0.0);
			ResetLocoMotionStartMoveTime();
			ResetLocoMotionEndMoveTime();
		}
		break;
	}
}

void APxcPlayerCharacter::OnEndMoveStarted()
{
	if (!IsValid(GetMesh())) return;

	if (m_eAnimBPType == EAnimBPType::DirectionTurned || m_eAnimBPType == EAnimBPType::ControllerTurned)
	{
		do {
			FVector&& vLeftFoot = GetMesh()->GetSocketLocation(m_LeftFootSocketName);
			FVector vLeftDown(vLeftFoot.X, vLeftFoot.Y, vLeftFoot.Z - m_fFootDownLength);
			FHitResult LeftHitResult;
			if (!UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), vLeftFoot, vLeftDown, m_tarrFootTraceTypes,
				false, TArray<AActor*>(), EDrawDebugTrace::Type::None, LeftHitResult, true)) break;

			FVector&& vRightFoot = GetMesh()->GetSocketLocation(m_RightFootSocketName);
			FVector vRightDown(vRightFoot.X, vRightFoot.Y, vRightFoot.Z - m_fFootDownLength);
			FHitResult RightHitResult;
			if (!UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), vRightFoot, vRightDown, m_tarrFootTraceTypes,
				false, TArray<AActor*>(), EDrawDebugTrace::Type::None, RightHitResult, true)) break;

			if (LeftHitResult.Distance > RightHitResult.Distance)
				m_eFootType = EEndMoveFootType::Left;
			else if (LeftHitResult.Distance < RightHitResult.Distance)
				m_eFootType = EEndMoveFootType::Right;
			else
				m_eFootType = EEndMoveFootType::Any;
		} while (false);
	}

	DeleEndMoveStarted.Broadcast();
}

void APxcPlayerCharacter::ResetLocoMotionStartMoveTime()
{
	m_fStartMoveTime = -1.0f;
	m_fStartMoveStamp = 0.0f;
}

void APxcPlayerCharacter::ResetLocoMotionEndMoveTime()
{
	m_fEndMoveTime = -1.0f;
	m_fEndMoveStamp = 0.0f;
}