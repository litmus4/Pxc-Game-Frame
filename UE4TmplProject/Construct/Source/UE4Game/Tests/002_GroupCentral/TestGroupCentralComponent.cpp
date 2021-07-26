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

	m_pSpawnedDirectActor = nullptr;
	m_pCameraActor = nullptr;
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
			pPawn->DisableInput(Cast<APlayerController>(pPawn->GetController()));
	}

	UGroupCentralTargetMgr* pManager = pGM->GetGroupCentralTargetMgr();
	check(pManager);
	pManager->SetCentralTarget(m_GroupName, 5.0f, 1.0f, 50.0f, 0.5f, 0.5f);
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
		SpawnParam.Name = TEXT("GroupCentralSpawnedDirect");
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
			SpawnParam.Name = TEXT("GroupCentralCamera");
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
	}), 3.0f, false);
	m_vecTimerCache.push_back(Timer1);

	FTimerHandle Timer2;
	GetWorld()->GetTimerManager().SetTimer(Timer2, FTimerDelegate::CreateLambda([pManager, this, Param]() {
		pManager->MoveCentralDirect(m_GroupName, Param.pMainActor);
		pManager->BlendCentralView(m_GroupName, Param.pMainActor);
	}), 6.0f, false);
	m_vecTimerCache.push_back(Timer2);
}

void UTestGroupCentralComponent::TickComponent(float fDeltaTime, ELevelTick eTickType, FActorComponentTickFunction* pThisTickFunction)
{
	Super::TickComponent(fDeltaTime, eTickType, pThisTickFunction);

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