// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Controllers/PxcPlayerController.h"
#include "Actors/Roles/PxcPlayerRole.h"

APxcPlayerRole* APxcPlayerController::GetPlayerRole()
{
	return m_pPlayerRole;
}

void APxcPlayerController::SetPlayerRole(APxcPlayerRole* pRole)
{
	check(IsValid(pRole));
	m_pPlayerRole = pRole;
	pRole->SetThePlayerCharacter(GetPawn<APxcPlayerCharacter>());
}