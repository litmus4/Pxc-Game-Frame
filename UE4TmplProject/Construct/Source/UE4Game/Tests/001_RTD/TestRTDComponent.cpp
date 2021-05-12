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
	pGroupMgr->CreateGroup(TEXT("RTD"), tarrUsages);

	FActorSpawnParameters Param;
	if (m_cSpawnedShowActor.Get())
	{
		Param.Name = TEXT("RTDSpawnedShowing");
		Param.Owner = GetOwner();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		m_pSpawnedShowActor = GetWorld()->SpawnActor(m_cSpawnedShowActor, &m_transSpawnedShowing, Param);
		if (m_pSpawnedShowActor)
			pGroupMgr->AddActorToGroup(m_pSpawnedShowActor, TEXT("RTD"));
	}

	if (m_cCameraActor.Get())
	{
		Param.Name = TEXT("RTDCamera");
		Param.Owner = GetOwner();
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		m_pCameraActor = GetWorld()->SpawnActor(m_cCameraActor, &m_transCamera, Param);
		if (m_pCameraActor)
			pGroupMgr->AddActorToGroup(m_pCameraActor, TEXT("RTD"));
	}
}

void UTestRTDComponent::RunCppTestNoParam()
{
	if (!IsValid(m_pSpawnedShowActor) || !IsValid(m_pCameraActor))
		return;

	APxcGameMode* pGM = GetWorld()->GetAuthGameMode<APxcGameMode>();
	if (!pGM) return;

	//FLAGJK_Test! SetViewTargetWithBlend

	URelativeTimeDilationMgr* pManager = pGM->GetRelativeTimeDilationMgr();
	check(pManager);
	//pManager
}