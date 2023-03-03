// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/002_GroupCentral/TestGroupCentralComponent.h"
#include "Framework/PxcGameMode.h"
#include "Framework/LevelMgrs/VirtualGroupMgr.h"
#include "Framework/LevelMgrs/GroupCentralTargetMgr.h"

UTestGroupCentralComponent::UTestGroupCentralComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_pDefaultDynamic = nullptr;
	m_pOwnerDynamic = nullptr;
	m_fHalfStageTime = 3.0f;

	m_pSpawnedDirectActor = nullptr;
	m_pCameraActor = nullptr;
	m_pRunningPawn = nullptr;
	m_iStage = 0;
	m_fCurTime = 0.0f;
	m_uSpawnCount = 0;
}

void UTestGroupCentralComponent::BeginPlay()
{
	Super::BeginPlay();

	APxcGameMode* pGM = GetWorld()->GetAuthGameMode<APxcGameMode>();
	if (!pGM) return;

	UVirtualGroupMgr* pGroupMgr = pGM->GetVirtualGroupMgr();
	check(pGroupMgr);
	TArray<EVirtualGroupUsage> tarrUsages;
	tarrUsages.Add(EVirtualGroupUsage::CentralTarget);
	pGroupMgr->CreateGroup(m_GroupName, tarrUsages);

	if (GetOwner())
		pGroupMgr->AddActorToGroup(GetOwner(), m_GroupName);
}

bool UTestGroupCentralComponent::CastParameter(const FSharedSignature& ParamSig, FTestGroupCentralParameter& OutParam)
{
	if (ParamSig.IsDerived<FTestGroupCentralParameter>())
	{
		OutParam = *ParamSig.GetDerivedPtr<FTestGroupCentralParameter>();
		return true;
	}
	return false;
}

void UTestGroupCentralComponent::RunCppTestWithParam(const FSharedSignature& ParamSig)
{
	if (!IsValid(GetOwner())) return;

	APxcGameMode* pGM = GetWorld()->GetAuthGameMode<APxcGameMode>();
	if (!pGM) return;
	FTestGroupCentralParameter Param;
	if (!CastParameter(ParamSig, Param)) return;

	UVirtualGroupMgr* pGroupMgr = pGM->GetVirtualGroupMgr();
	check(pGroupMgr);
	if (Param.pMainActor)
	{
		pGroupMgr->AddActorToGroup(Param.pMainActor, m_GroupName);
		APawn* pPawn = Cast<APawn>(Param.pMainActor);
		if (pPawn)
		{
			pPawn->DisableInput(Cast<APlayerController>(pPawn->GetController()));
			m_pRunningPawn = pPawn;
		}
	}
	m_iStage = 1;
	m_fCurTime = 0.0f;
	m_vStartLoc = GetOwner()->GetActorLocation();

	UGroupCentralTargetMgr* pManager = pGM->GetGroupCentralTargetMgr();
	check(pManager);
	pManager->SetCentralTarget(m_GroupName, 5.0f, 1.0f, 40.0, 0.5f, 0.5f);
	FGroupCentralDirectChangeDelegate DeleDirect;
	pManager->SetCentralDirect(m_GroupName, 1.0f, m_pDefaultDynamic, DeleDirect);
	pManager->AddActorDirect(m_GroupName, GetOwner(), 1.0f, m_pOwnerDynamic);
	pManager->AddActorDirect(m_GroupName, Param.pMainActor, 1.5f);

	if (m_cSpawnedDirectActor.Get())
	{
		FVector vLoc;
		pManager->GetDirectTarget(m_GroupName, vLoc);
		FTransform trans(FRotator::ZeroRotator, vLoc);
		FActorSpawnParameters SpawnParam;
		SpawnParam.Name = FName(FString::Printf(TEXT("GroupCentralSpawnedDirect%d"), ++m_uSpawnCount));
		SpawnParam.Owner = GetOwner();
		SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		m_pSpawnedDirectActor = GetWorld()->SpawnActor(m_cSpawnedDirectActor, &trans, SpawnParam);
	}

	FTimerHandle Timer1;
	GetWorld()->GetTimerManager().SetTimer(Timer1, FTimerDelegate::CreateLambda([pManager, this, Param]() {
		if (m_cCameraActor.Get())
		{
			FVector vLoc;
			pManager->GetCentralTarget(m_GroupName, vLoc);
			FTransform trans(FRotator::ZeroRotator, vLoc);
			FActorSpawnParameters SpawnParam;
			SpawnParam.Name = FName(FString::Printf(TEXT("GroupCentralCamera%d"), m_uSpawnCount));
			SpawnParam.Owner = GetOwner();
			SpawnParam.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			m_pCameraActor = GetWorld()->SpawnActor(m_cCameraActor, &trans, SpawnParam);
			if (m_pCameraActor)
			{
				FGroupCentralViewChangeDelegate DeleView;
				pManager->SetCentralView(m_GroupName, 0.5f, EViewTargetBlendFunction::VTBlend_Cubic,
					m_pCameraActor, Param.pController, DeleView);
			}
		}

		pManager->MoveCentralDirect(m_GroupName, GetOwner());
	}), m_fHalfStageTime, false);
	m_vecTimerCache.push_back(Timer1);

	FTimerHandle Timer2;
	GetWorld()->GetTimerManager().SetTimer(Timer2, FTimerDelegate::CreateLambda([pManager, this, Param]() {
		m_iStage = 2;
		m_fCurTime = 0.0f;
		pManager->MoveCentralDirect(m_GroupName, Param.pMainActor);
		pManager->BlendCentralView(m_GroupName, Param.pMainActor);
	}), GetStageTime(1), false);
	m_vecTimerCache.push_back(Timer2);
}

void UTestGroupCentralComponent::MakeParameterByOverlappingActor(AActor* pActor, FSharedSignature& OutSig)
{
	APawn* pPawn = CastChecked<APawn>(pActor);
	TSharedPtr<FTestGroupCentralParameter> pParam = MakeShared<FTestGroupCentralParameter>();
	pParam->pMainActor = pActor;
	pParam->pController = pPawn->GetController<APlayerController>();
	OutSig = FSharedSignature(*pParam);
}

bool UTestGroupCentralComponent::IsRunning()
{
	return (m_iStage > 0);
}

void UTestGroupCentralComponent::TickComponent(float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* pThisTickFunction)
{
	Super::TickComponent(fDeltaTime, eTickType, pThisTickFunction);

	if (m_iStage && IsValid(GetOwner()))
	{
		m_fCurTime += fDeltaTime;
		float fMaxTime = GetStageTime(m_iStage);
		bool bAllEnd = false;

		if (fMaxTime < -0.5f)
			bAllEnd = true;
		else
		{
			while (m_fCurTime >= fMaxTime)
			{
				m_fCurTime -= fMaxTime;
				m_iStage++;
				fMaxTime = GetStageTime(m_iStage);
				if (fMaxTime < -0.5f)
				{
					bAllEnd = true;
					break;
				}
			}
		}

		if (!bAllEnd)
		{
			FVector vStart, vEnd;
			switch (m_iStage)
			{
			case 1:
				vStart = m_vStartLoc; vEnd = m_vStartLoc + m_vOwnerRunning;
				break;
			case 2:
				vStart = m_vStartLoc + m_vOwnerRunning; vEnd = m_vStartLoc;
				break;
			}
			float fRatio = m_fCurTime / fMaxTime;
			GetOwner()->SetActorLocation(FMath::Lerp(vStart, vEnd, fRatio));
		}
		else
			Final();
	}

	if (m_pSpawnedDirectActor)
	{
		APxcGameMode* pGM = GetWorld()->GetAuthGameMode<APxcGameMode>();
		if (!pGM) return;
		UGroupCentralTargetMgr* pManager = pGM->GetGroupCentralTargetMgr();
		check(pManager);
		FVector vLoc;
		if (pManager->GetDirectTarget(m_GroupName, vLoc))
			m_pSpawnedDirectActor->SetActorLocation(vLoc);
	}
}

float UTestGroupCentralComponent::GetStageTime(int32 iStage)
{
	float* pfTime = m_tmapStageTimes.Find(iStage);
	return (pfTime ? *pfTime : -1.0f);
}

void UTestGroupCentralComponent::Final()
{
	m_iStage = 0;
	if (IsValid(GetOwner()))
		GetOwner()->SetActorLocation(m_vStartLoc);

	APxcGameMode* pGM = GetWorld()->GetAuthGameMode<APxcGameMode>();
	if (!pGM) return;
	UGroupCentralTargetMgr* pManager = pGM->GetGroupCentralTargetMgr();
	check(pManager);
	pManager->ResetCentralTarget(m_GroupName);
	UVirtualGroupMgr* pGroupMgr = pGM->GetVirtualGroupMgr();
	check(pGroupMgr);
	if (m_pRunningPawn)
	{
		pGroupMgr->RemoveActorFromGroup(m_pRunningPawn, m_GroupName);
		m_pRunningPawn->EnableInput(Cast<APlayerController>(m_pRunningPawn->GetController()));
		m_pRunningPawn = nullptr;
	}

	if (m_pSpawnedDirectActor)
	{
		m_pSpawnedDirectActor->Destroy();
		m_pSpawnedDirectActor = nullptr;
	}
	if (m_pCameraActor)
	{
		m_pCameraActor->Destroy();
		m_pCameraActor = nullptr;
	}
	m_vecTimerCache.clear();
}