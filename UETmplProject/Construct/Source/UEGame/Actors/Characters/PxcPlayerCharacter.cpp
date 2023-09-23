// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "EngineRelated/Animations/PxcCharacterTrajectoryComponent.h"
#include "PoseSearchLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Actors/Controllers/PxcPlayerController.h"
#include "PxcUtil/DateTime.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/Scattered.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

APxcPlayerCharacter::APxcPlayerCharacter()
{
	m_pSpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	m_pSpringArmComp->bUsePawnControlRotation = true;
	m_pSpringArmComp->SetupAttachment(RootComponent);

	m_pCameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	m_pCameraComp->SetupAttachment(m_pSpringArmComp);

	m_pTrajectoryComp = nullptr;

	m_bRootMotion = false;
	m_eAnimBPType = EAnimBPType::DirectionTurned;
	m_pStartMoveCurve = nullptr;
	m_pEndMoveCurve = nullptr;
	m_dFootDownLength = 100.0;

	m_vFocus = FVector::ZeroVector;
	m_eMotionState = EMotionState::Idle;
	m_fStartMoveMaxTime = 0.0f;
	m_fEndMoveMaxTime = 0.0f;
	m_fStartMoveLength = -1.0f;
	m_fEndMoveLength = -1.0f;
	m_eFootType = EEndMoveFootType::Any;

	m_v2Axis.Set(0.0, 0.0);
	m_v2LastAxis.Set(0.0, 0.0);
	m_pTrajectory = nullptr;
	m_fStartMoveTime = -1.0f;
	m_fEndMoveTime = -1.0f;
	m_fStartMoveStamp = 0.0f;
	m_fEndMoveStamp = 0.0f;
	m_fStartToEndValue = -1.0f;
	m_fEndToStartValue = -1.0f;
	m_bEndMoveStarted = false;
}

void APxcPlayerCharacter::BindUniversalEventByType(EPlayerUniEventType eType, FPlayerUniversalOneDelegate DeleCall)
{
	std::unordered_map<EPlayerUniEventType, FPlayerUniversalEventDelegate>::iterator iter = m_mapUniversalEvents.find(eType);
	if (iter == m_mapUniversalEvents.end())
	{
		FPlayerUniversalEventDelegate DeleList;
		DeleList.Add(DeleCall);
		m_mapUniversalEvents.insert(std::make_pair(eType, DeleList));
	}
	else
		iter->second.Add(DeleCall);
}

void APxcPlayerCharacter::UnbindUniversalEventsFromType(EPlayerUniEventType eType, UObject* pObject)
{
	std::unordered_map<EPlayerUniEventType, FPlayerUniversalEventDelegate>::iterator iter = m_mapUniversalEvents.find(eType);
	if (iter != m_mapUniversalEvents.end())
	{
		iter->second.RemoveAll(pObject);
		if (!iter->second.IsBound())
			m_mapUniversalEvents.erase(iter);
	}
}

void APxcPlayerCharacter::UnbindAllUniversalEventsFromType(EPlayerUniEventType eType)
{
	std::unordered_map<EPlayerUniEventType, FPlayerUniversalEventDelegate>::iterator iter = m_mapUniversalEvents.find(eType);
	if (iter != m_mapUniversalEvents.end())
	{
		iter->second.Clear();
		m_mapUniversalEvents.erase(iter);
	}
}

void APxcPlayerCharacter::CallUniversalEventByType(EPlayerUniEventType eType)
{
	std::unordered_map<EPlayerUniEventType, FPlayerUniversalEventDelegate>::iterator iter = m_mapUniversalEvents.find(eType);
	if (iter != m_mapUniversalEvents.end())
		iter->second.Broadcast(this);
}

void APxcPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	float fTempMin = 0.0f;
	if (IsValid(m_pStartMoveCurve))
		m_pStartMoveCurve->FloatCurve.GetTimeRange(fTempMin, m_fStartMoveMaxTime);
	if (IsValid(m_pEndMoveCurve))
		m_pEndMoveCurve->FloatCurve.GetTimeRange(fTempMin, m_fEndMoveMaxTime);

	if (m_eAnimBPType == EAnimBPType::MotionMatchingDT || m_eAnimBPType == EAnimBPType::MotionMatchingCT)
		m_pTrajectoryComp = FindComponentByClass<UPxcCharacterTrajectoryComponent>();
}

void APxcPlayerCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (m_eAnimBPType == EAnimBPType::MotionMatchingDT || m_eAnimBPType == EAnimBPType::MotionMatchingCT)
	{
		m_pTrajectoryComp = nullptr;
		SAFE_DELETE(m_pTrajectory)
	}

	Super::EndPlay(EndPlayReason);
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

float APxcPlayerCharacter::GetMachineStateLength(UAnimInstance* pABP, FName MachineName, EMotionState eMotionState)
{
	if (!IsValid(pABP))
	{
		check(GetMesh());
		pABP = GetMesh()->GetAnimInstance();
	}

	int32 iMachineIndex = pABP->GetStateMachineIndex(MachineName);
	if (iMachineIndex != INDEX_NONE)
	{
		const FAnimNode_StateMachine* pMachine = pABP->GetStateMachineInstance(iMachineIndex);
		if (!pMachine) return -1.0f;

		FName&& CurStateName = pMachine->GetCurrentStateName();
		FName&& MotionName = FName(StaticEnum<EMotionState>()->GetNameByIndex((int32)eMotionState).ToString().RightChop(14));
		if (CurStateName != NAME_None && CurStateName == MotionName)
			return pABP->GetRelevantAnimLength(iMachineIndex, pMachine->GetCurrentState());
	}
	return -1.0f;
}

void APxcPlayerCharacter::RunLocoMotionEndMoveInput()
{
	float fValue = 1.0f;
	if (IsValid(m_pEndMoveCurve))
	{
		if (m_fEndMoveTime >= -0.5f && m_fEndMoveTime <= m_fEndMoveLength)
		{
			float fCurTime = PxcUtil::ExactTime::GetFloatTime();
			m_fEndMoveTime = fCurTime - m_fEndMoveStamp;
			float fScaledMoveTime = m_fEndMoveTime / m_fEndMoveLength * m_fEndMoveMaxTime;
			fValue = FMath::Clamp(m_pEndMoveCurve->GetFloatValue(FMath::Min(fScaledMoveTime, m_fEndMoveMaxTime)), 0.0f, 1.0f);
			BuildStartToEndBridge(fValue);//FLAGJK 还存在StartToEnd搭桥后fValue以后永远是搭桥值的问题
			//*测试临时
			UE_LOG(LogTemp, Warning, TEXT("$$$$$$$$ Value: %.3f"), fValue);
			//*/
		}
		else if (m_fEndMoveTime < -0.5f)
		{
			m_fEndMoveTime = 0.0f;
			m_fEndMoveStamp = PxcUtil::ExactTime::GetFloatTime();
			m_fEndMoveLength = GetMachineStateLength(nullptr, m_AnimMachineName, m_eMotionState);
			//float fScaledMoveTime = m_fEndMoveTime / m_fEndMoveLength * m_fEndMoveMaxTime;
			fValue = FMath::Clamp(m_pEndMoveCurve->GetFloatValue(FMath::Min(m_fEndMoveTime, m_fEndMoveMaxTime)), 0.0f, 1.0f);
			BuildStartToEndBridge(fValue);
		}
		else
		{
			ResetLocoMotionEndMoveTime();
			fValue = 0.0f;
		}
	}

	if (m_eAnimBPType == EAnimBPType::DirectionTurned || m_eAnimBPType == EAnimBPType::MotionMatchingDT)
		AddMovementInput(GetActorForwardVector(), fValue);
	else
		AddMovementInput(m_vFocus.GetSafeNormal(), fValue);
}

bool APxcPlayerCharacter::GetMotionTrajectory(FPoseSearchQueryTrajectory& OutTrajectory)
{
	if (m_pTrajectory)
	{
		OutTrajectory = *m_pTrajectory;
		return true;
	}
	return false;
}

void APxcPlayerCharacter::PossessedBy(AController* pNewController)
{
	Super::PossessedBy(pNewController);

	APxcPlayerController* pController = Cast<APxcPlayerController>(pNewController);
	if (pController)
	{
		pController->SpawnPlayerRole();
		//
	}
}

void APxcPlayerCharacter::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

	if (m_bEndMoveStarted)
	{
		OnEndMoveStarted();
		m_bEndMoveStarted = false;
	}

	if (IsValid(m_pTrajectoryComp))
	{
		if (!m_pTrajectory)
			m_pTrajectory = new FPoseSearchQueryTrajectory();
		if (m_pTrajectory)
		{
			if (m_bRootMotion)
			{
				check(IsValid(GetMesh()));
				FPoseSearchQueryTrajectory&& ActorTraj = m_pTrajectoryComp->GetTrajectory();
				//*m_pTrajectory = UMotionTrajectoryBlueprintLibrary::MakeTrajectoryRelativeToComponent(ActorTraj, GetMesh());
				//FLAGJK UPoseSearchLibrary::ProcessTrajectory私有
				//*m_pTrajectory = UPoseSearchLibrary::ProcessTrajectory(ActorTraj, GetMesh()->GetComponentTransform(), 0.0f, 0.0f, 0.0f);
				*m_pTrajectory = ActorTraj;
			}
			else
				*m_pTrajectory = m_pTrajectoryComp->GetTrajectory();
		}
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
		if (!m_bRootMotion)
		{
			m_v2LastAxis.Set(0.0, 0.0);
			if (IsValid(m_pStartMoveCurve))
			{
				m_fStartMoveTime = 0.0f;
				m_fStartMoveStamp = PxcUtil::ExactTime::GetFloatTime();
			}
			CheckAndBeginEndToStartBridge();
		}
		m_eMotionState = EMotionState::StartMove;
	}
	else if (m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Move ||
		m_eMotionState == EMotionState::StartMove))
	{
		if (!m_bRootMotion)
		{
			m_v2LastAxis = v2LastAxis;
			CheckAndBeginStartToEndBridge();
		}
		m_eMotionState = EMotionState::EndMove;
		m_bEndMoveStarted = true;
	}

	if (m_v2Axis.IsNearlyZero() && m_v2LastAxis.IsNearlyZero())
		return;

	if (m_eAnimBPType == EAnimBPType::DirectionTurned || m_eAnimBPType == EAnimBPType::MotionMatchingDT)
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
			if (m_fStartMoveLength < -0.5f)
				m_fStartMoveLength = GetMachineStateLength(nullptr, m_AnimMachineName, m_eMotionState);

			if (m_fStartMoveTime >= -0.5f && m_fStartMoveTime <= (m_fStartMoveLength >= -0.5f ? m_fStartMoveLength : m_fStartMoveMaxTime))
			{
				float fCurTime = PxcUtil::ExactTime::GetFloatTime();
				m_fStartMoveTime = fCurTime - m_fStartMoveStamp;
				if (!m_v2Axis.IsNearlyZero())
				{
					bool bNega = (fValue < 0.0f);
					if (m_fStartMoveLength >= -0.5f)
					{
						float fScaledMoveTime = m_fStartMoveTime / m_fStartMoveLength * m_fStartMoveMaxTime;
						fValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(fScaledMoveTime, m_fStartMoveMaxTime)), 0.0f, 1.0f);
					}
					else
						fValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(m_fStartMoveTime, m_fStartMoveMaxTime)), 0.0f, 1.0f);
					BuildEndToStartBridge(fValue);
					fValue = (bNega ? -fValue : fValue);
				}
			}
			else
				ResetLocoMotionStartMoveTime();
		}
		AddMovementInput(GetActorForwardVector(),
			(m_eAnimBPType == EAnimBPType::DirectionTurned || m_eAnimBPType == EAnimBPType::MotionMatchingDT ?
				FMath::Abs(fValue) : fValue));
	}
}

void APxcPlayerCharacter::OnMoveRight(float fValue)
{
	FVector2D v2LastAxis = m_v2Axis;
	m_v2Axis.Y = fValue;
	if (!m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Idle ||
		m_eMotionState == EMotionState::EndMove))
	{
		if (!m_bRootMotion)
		{
			m_v2LastAxis.Set(0.0, 0.0);
			if (IsValid(m_pStartMoveCurve))
			{
				m_fStartMoveTime = 0.0f;
				m_fStartMoveStamp = PxcUtil::ExactTime::GetFloatTime();
			}
			CheckAndBeginEndToStartBridge();
		}
		m_eMotionState = EMotionState::StartMove;
	}
	else if (m_v2Axis.IsNearlyZero() && (m_eMotionState == EMotionState::Move ||
		m_eMotionState == EMotionState::StartMove))
	{
		if (!m_bRootMotion)
		{
			m_v2LastAxis = v2LastAxis;
			CheckAndBeginStartToEndBridge();
		}
		m_eMotionState = EMotionState::EndMove;
		m_bEndMoveStarted = true;
	}

	if (m_v2Axis.IsNearlyZero() && m_v2LastAxis.IsNearlyZero())
		return;

	if (m_eAnimBPType == EAnimBPType::DirectionTurned || m_eAnimBPType == EAnimBPType::MotionMatchingDT)
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
			if (m_fStartMoveLength < -0.5f)
				m_fStartMoveLength = GetMachineStateLength(nullptr, m_AnimMachineName, m_eMotionState);

			if (m_fStartMoveTime >= -0.5f && m_fStartMoveTime <= (m_fStartMoveLength >= -0.5f ? m_fStartMoveLength : m_fStartMoveMaxTime))
			{
				float fCurTime = PxcUtil::ExactTime::GetFloatTime();
				m_fStartMoveTime = fCurTime - m_fStartMoveStamp;
				if (!m_v2Axis.IsNearlyZero())
				{
					bool bNega = (fValue < 0.0f);
					if (m_fStartMoveLength >= -0.5f)
					{
						float fScaledMoveTime = m_fStartMoveTime / m_fStartMoveLength * m_fStartMoveMaxTime;
						fValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(fScaledMoveTime, m_fStartMoveMaxTime)), 0.0f, 1.0f);
					}
					else
						fValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(m_fStartMoveTime, m_fStartMoveMaxTime)), 0.0f, 1.0f);
					BuildEndToStartBridge(fValue);
					fValue = (bNega ? -fValue : fValue);
				}
			}
			else
				ResetLocoMotionStartMoveTime();
		}
		if (m_eAnimBPType == EAnimBPType::DirectionTurned || m_eAnimBPType == EAnimBPType::MotionMatchingDT)
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

	if (m_eAnimBPType == EAnimBPType::DirectionTurned || m_eAnimBPType == EAnimBPType::ControllerTurned ||
		m_eAnimBPType == EAnimBPType::MotionMatchingDT || m_eAnimBPType == EAnimBPType::MotionMatchingCT)
	{
		do {
			FVector&& vLeftFoot = GetMesh()->GetSocketLocation(m_LeftFootSocketName);
			FVector vLeftDown(vLeftFoot.X, vLeftFoot.Y, vLeftFoot.Z - m_dFootDownLength);
			FHitResult LeftHitResult;
			if (!UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), vLeftFoot, vLeftDown, m_tarrFootTraceTypes,
				false, TArray<AActor*>(), EDrawDebugTrace::Type::None, LeftHitResult, true)) break;

			FVector&& vRightFoot = GetMesh()->GetSocketLocation(m_RightFootSocketName);
			FVector vRightDown(vRightFoot.X, vRightFoot.Y, vRightFoot.Z - m_dFootDownLength);
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

void APxcPlayerCharacter::CheckAndBeginStartToEndBridge()
{
	if (m_eMotionState != EMotionState::StartMove || !IsValid(m_pStartMoveCurve))
		return;

	m_fStartToEndValue = 1.0f;
	if (m_fStartMoveTime >= -0.5f)
	{
		if (m_fStartMoveLength >= -0.5f)
		{
			float fScaledMoveTime = m_fStartMoveTime / m_fStartMoveLength * m_fStartMoveMaxTime;
			m_fStartToEndValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(fScaledMoveTime, m_fStartMoveMaxTime)), 0.0f, 1.0f);
		}
		else
			m_fStartToEndValue = FMath::Clamp(m_pStartMoveCurve->GetFloatValue(FMath::Min(m_fStartMoveTime, m_fStartMoveMaxTime)), 0.0f, 1.0f);
		ResetLocoMotionStartMoveTime();
	}
}

bool APxcPlayerCharacter::BuildStartToEndBridge(float& fValue)
{
	if (m_fStartToEndValue >= -0.5f)
	{
		if (fValue > m_fStartToEndValue)
		{
			fValue = m_fStartToEndValue;
			return true;
		}
		else
			m_fStartToEndValue = -1.0f;
	}
	return false;
}

void APxcPlayerCharacter::CheckAndBeginEndToStartBridge()
{
	if (m_eMotionState != EMotionState::EndMove || !IsValid(m_pEndMoveCurve))
		return;

	m_fEndToStartValue = 0.0f;
	if (m_fEndMoveTime >= -0.5f)
	{
		if (m_fEndMoveLength >= -0.5f)
		{
			float fScaledMoveTime = m_fEndMoveTime / m_fEndMoveLength * m_fEndMoveMaxTime;
			m_fEndToStartValue = FMath::Clamp(m_pEndMoveCurve->GetFloatValue(FMath::Min(fScaledMoveTime, m_fEndMoveMaxTime)), 0.0f, 1.0f);
		}
		else
			m_fEndToStartValue = FMath::Clamp(m_pEndMoveCurve->GetFloatValue(FMath::Min(m_fEndMoveTime, m_fEndMoveMaxTime)), 0.0f, 1.0f);
		ResetLocoMotionEndMoveTime();
	}
}

bool APxcPlayerCharacter::BuildEndToStartBridge(float& fValue)
{
	if (m_fEndToStartValue >= -0.5f)
	{
		if (fValue < m_fEndToStartValue)
		{
			fValue = m_fEndToStartValue;
			return true;
		}
		else
			m_fEndToStartValue = -1.0f;
	}
	return false;
}

void APxcPlayerCharacter::ResetLocoMotionStartMoveTime()
{
	m_fStartMoveTime = -1.0f;
	m_fStartMoveStamp = 0.0f;
	m_fStartMoveLength = -1.0f;
}

void APxcPlayerCharacter::ResetLocoMotionEndMoveTime()
{
	m_fEndMoveTime = -1.0f;
	m_fEndMoveStamp = 0.0f;
	m_fEndMoveLength = -1.0f;
}