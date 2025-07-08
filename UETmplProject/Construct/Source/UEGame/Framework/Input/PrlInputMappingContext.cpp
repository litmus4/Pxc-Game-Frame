// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Input/PrlInputMappingContext.h"
#include "EnhancedInputSubsystems.h"

bool UPrlInputMappingContext::SetSubsystemFromController(APlayerController* pPlayerController)
{
	if (IsValid(pPlayerController))
	{
		m_pLocalPlayer = pPlayerController->GetLocalPlayer();
		if (IsValid(m_pLocalPlayer))
		{
			m_pSubsystem =  m_pLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			return IsValid(m_pSubsystem);
		}
	}
	return false;
}

void UPrlInputMappingContext::AddActionMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps)
{
	if (!KeyMapping.Action || KeyMapping.Action->ValueType != EInputActionValueType::Boolean)
		return;

	Mappings.AddUnique(KeyMapping);
	if (bForceRebuildKeymaps)
	{
		check(IsValid(m_pSubsystem));
		m_pSubsystem->RequestRebuildControlMappings();
	}
}

void UPrlInputMappingContext::RemoveActionMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps)
{
	if (!KeyMapping.Action || KeyMapping.Action->ValueType != EInputActionValueType::Boolean)
		return;

	for (int32 iIndex = Mappings.Num() - 1; iIndex >= 0; --iIndex)
	{
		if (Mappings[iIndex] == KeyMapping)
		{
			Mappings.RemoveAt(iIndex);
			// we don't break because the mapping may have been in the array twice
		}
	}
	if (bForceRebuildKeymaps)
	{
		check(IsValid(m_pSubsystem));
		m_pSubsystem->RequestRebuildControlMappings();
	}
}