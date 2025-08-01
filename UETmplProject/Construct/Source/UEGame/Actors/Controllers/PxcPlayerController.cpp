// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Controllers/PxcPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Actors/Roles/PxcPlayerRole.h"
#include "Actors/Characters/PxcPlayerCharacter.h"
#include "Framework/Input/PrlInputMappingContext.h"
#include "Framework/PXCycleInstance.h"

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
	if (IsValid(m_pPrlInputMappingContext) && m_pPrlInputMappingContext->SetSubsystemFromController(this))
	{
		UPXCycleInstance* pGI = GetWorld()->GetGameInstance<UPXCycleInstance>();
		check(pGI);
#if WITH_EDITOR
		if (!m_bUseDefaultPIMCOnFirstPossess && !m_bUseDefaultPIMCAlways)
		{
#endif
			if (!pGI->m_bPIMCConfigLoaded)
			{
				pGI->SetAndCacheDefaultPIMC(m_pPrlInputMappingContext);
				pGI->m_bPIMCConfigLoaded = m_pPrlInputMappingContext->LoadKeyMappings();
			}
#if WITH_EDITOR
		}
		else
		{
			if (!m_bUseDefaultPIMCAlways)
				m_bUseDefaultPIMCOnFirstPossess = false;
			if (!pGI->m_pPrlIMC)
				pGI->m_pPrlIMC = m_pPrlInputMappingContext;
		}
#endif
		m_pPrlInputMappingContext->m_pSubsystem->AddMappingContext(m_pPrlInputMappingContext, 0);
	}

	Super::OnPossess(pPawn);
}

void APxcPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (IsValid(m_pPrlInputMappingContext) && IsValid(m_pPrlInputMappingContext->m_pSubsystem))
		m_pPrlInputMappingContext->m_pSubsystem->RemoveMappingContext(m_pPrlInputMappingContext);
}