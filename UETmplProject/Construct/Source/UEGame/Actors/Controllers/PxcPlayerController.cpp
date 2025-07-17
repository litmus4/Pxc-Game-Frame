// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Controllers/PxcPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/Roles/PxcPlayerRole.h"
#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Framework/Input/PrlInputMappingContext.h"

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

void APxcPlayerController::OnPossess(APawn* pPawn)
{
	Super::OnPossess(pPawn);

	if (IsValid(m_pPrlInputMappingContext) && m_pPrlInputMappingContext->SetSubsystemFromController(this))
		m_pPrlInputMappingContext->m_pSubsystem->AddMappingContext(m_pPrlInputMappingContext, 0);
}

void APxcPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (IsValid(m_pPrlInputMappingContext) && IsValid(m_pPrlInputMappingContext->m_pSubsystem))
		m_pPrlInputMappingContext->m_pSubsystem->RemoveMappingContext(m_pPrlInputMappingContext);
}