// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/001_RTD/TestRTDComponent.h"
#include "Framework/PxcGameMode.h"
#include "Framework/LevelMgrs/VirtualGroupMgr.h"
#include "Framework/LevelMgrs/RelativeTimeDilationMgr.h"

void UTestRTDComponent::BeginPlay()
{
	Super::BeginPlay();

	APxcGameMode* pGM = GetWorld()->GetAuthGameMode<APxcGameMode>();
	if (!pGM) return;

	UVirtualGroupMgr* pGroupMgr = pGM->GetVirtualGroupMgr();
	check(pGroupMgr);
	TArray<EVirtualGroupUsage> tarrUsages;
	tarrUsages.Add(EVirtualGroupUsage::RelativeTimeDilation);
	pGroupMgr->CreateGroup(m_GroupName, tarrUsages);

	FActorSpawnParameters Param;
	if (m_cSpawnedShowActor.Get())
	{
		Param.Name = TEXT("RTDSpawnedShowing");
		Param.Owner = GetOwner();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		m_pSpawnedShowActor = GetWorld()->SpawnActor(m_cSpawnedShowActor, &m_transSpawnedShowing, Param);
		if (m_pSpawnedShowActor)
			pGroupMgr->AddActorToGroup(m_pSpawnedShowActor, m_GroupName);
	}

	if (m_cCameraActor.Get())
	{
		Param.Name = TEXT("RTDCamera");
		Param.Owner = GetOwner();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		m_pCameraActor = GetWorld()->SpawnActor(m_cCameraActor, &m_transCamera, Param);
	}

	if (GetOwner())
		pGroupMgr->AddActorToGroup(GetOwner(), m_GroupName);
}

bool UTestRTDComponent::CastParameter(const FSharedSignature& ParamSig, FTestRTDParameter& OutParam)
{
	if (ParamSig.IsDerived<FTestRTDParameter>())
	{
		OutParam = *ParamSig.GetDerivedPtr<FTestRTDParameter>();
		return true;
	}
	return false;
}

void UTestRTDComponent::RunCppTestWithParam(const FSharedSignature& ParamSig)
{
	if (!IsValid(m_pSpawnedShowActor) || !IsValid(m_pCameraActor) || !IsValid(GetOwner()))
		return;

	APxcGameMode* pGM = GetWorld()->GetAuthGameMode<APxcGameMode>();
	if (!pGM) return;

	FTestRTDParameter Param;
	if (CastParameter(ParamSig, Param))
	{
		if (IsValid(Param.pController))
			Param.pController->SetViewTargetWithBlend(m_pCameraActor, 0.5f);
	}

	URelativeTimeDilationMgr* pManager = pGM->GetRelativeTimeDilationMgr();
	check(pManager);
	m_iEndCount = 0;
	
	FTimerHandle Timer1;
	GetWorld()->GetTimerManager().SetTimer(Timer1, FTimerDelegate::CreateLambda([pManager, this]() {
		//FLAGJK_Test ����켣

		FTimeDilationEndDelegate DeleCamera;
		DeleCamera.BindDynamic(this, &UTestRTDComponent::OnCameraDilationEnded);
		pManager->SetActorDilation(m_pCameraActor, 6.0f, 0.5f, 0.5f, 1.0f, m_pCameraDynamic, 1, true, DeleCamera);

		FTimeDilationEndDelegate DeleGroup;
		DeleGroup.BindDynamic(this, &UTestRTDComponent::OnGroupDilationEnded);
		pManager->SetGroupDilation(m_GroupName, 6.0f, 0.0f, 0.0f, 0.5f, nullptr, 1, false, DeleGroup);

		FTimeDilationEndDelegate DeleSpawned;
		DeleSpawned.BindDynamic(this, &UTestRTDComponent::OnSpawnedDilationEnded);
		pManager->SetActorDilation(m_pSpawnedShowActor, 6.0f, 0.0f, 0.0f, 0.7f, nullptr, 1, false, DeleSpawned);
	}), 0.6f, false);
	m_vecTimerCache.push_back(Timer1);

	FTimerHandle Timer2;
	GetWorld()->GetTimerManager().SetTimer(Timer2, FTimerDelegate::CreateLambda([pManager, this]() {
		FTimeDilationEndDelegate DeleGlobal;
		DeleGlobal.BindDynamic(this, &UTestRTDComponent::OnGlobalDilationEnded);
		pManager->SetGlobalDilation(3.0f, 0.0f, 0.0f, 0.7f, nullptr, 1, DeleGlobal);
	}), 3.6f, false);
	m_vecTimerCache.push_back(Timer2);
}

void UTestRTDComponent::MakeParameterByOverlappingActor(AActor* pActor, FSharedSignature& OutSig)
{
	APawn* pPawn = CastChecked<APawn>(pActor);
	TSharedPtr<FTestRTDParameter> pParam = MakeShared<FTestRTDParameter>();
	pParam->pController = pPawn->GetController<APlayerController>();
	OutSig = FSharedSignature(*pParam);
}

void UTestRTDComponent::OnCameraDilationEnded(bool bCanceled)
{
	m_iEndCount++;
	CheckFinal();
}

void UTestRTDComponent::OnGroupDilationEnded(bool bCanceled)
{
	m_iEndCount++;
	CheckFinal();
}

void UTestRTDComponent::OnSpawnedDilationEnded(bool bCanceled)
{
	m_iEndCount++;
	CheckFinal();
}

void UTestRTDComponent::OnGlobalDilationEnded(bool bCanceled)
{
	m_iEndCount++;
	CheckFinal();
}

void UTestRTDComponent::CheckFinal()
{
	if (m_iEndCount < 5) return;

	//
}