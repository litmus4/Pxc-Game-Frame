// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Controllers/PxcPlayerController.h"
#include "Actors/Roles/PxcPlayerRole.h"

APxcPlayerRole* APxcPlayerController::GetPlayerRole()
{
	return m_pPlayerRole;
}

void APxcPlayerController::SpawnPlayerRole()
{
	FActorSpawnParameters Param;
	Param.Owner = this;
	m_pPlayerRole = GetWorld()->SpawnActor<APxcPlayerRole>(Param);
	check(IsValid(m_pPlayerRole));
	m_pPlayerRole->SetThePlayerCharacter(GetPawn<APxcPlayerCharacter>());
}