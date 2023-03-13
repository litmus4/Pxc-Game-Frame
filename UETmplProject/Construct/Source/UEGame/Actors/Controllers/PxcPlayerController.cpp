// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Controllers/PxcPlayerController.h"
#include "Actors/Roles/PxcPlayerRole.h"
#include "Actors/Characters/PxcPlayerCharacter.h"

APxcPlayerRole* APxcPlayerController::GetPlayerRole()
{
	return m_pPlayerRole;
}

void APxcPlayerController::SpawnPlayerRole()
{
	FActorSpawnParameters Param;
	Param.Owner = this;
	if (m_cPlayerRole)
		m_pPlayerRole = GetWorld()->SpawnActor<APxcPlayerRole>(m_cPlayerRole, Param);
	else
		m_pPlayerRole = GetWorld()->SpawnActor<APxcPlayerRole>(Param);
	check(IsValid(m_pPlayerRole));
	m_pPlayerRole->SetThePlayerCharacter(GetPawn<APxcPlayerCharacter>());
}