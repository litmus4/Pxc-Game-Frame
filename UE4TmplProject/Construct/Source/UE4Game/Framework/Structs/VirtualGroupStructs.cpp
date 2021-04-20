// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Structs/VirtualGroupStructs.h"
#include "../PxcGameMode.h"
#include "../LevelMgrs/GroupCentralTargetMgr.h"

void FVirtGrpCentralFeature::OnActorUpdated(UWorld* pWorld, TSet<AActor*>& tsetActors)
{
	if (pWorld)
	{
		APxcGameMode* pGM = pWorld->GetAuthGameMode<APxcGameMode>();
		check(pGM);
		UGroupCentralTargetMgr* pManager = pGM->GetGroupCentralTargetMgr();
		check(pManager);
		pManager->UpdateCentralTarget(GroupName, nullptr, &tsetActors);
	}
}