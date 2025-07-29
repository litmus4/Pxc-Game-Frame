// Fill out your copyright notice in the Description page of Project Settings.


#include "PrlEnhancedInputConfig.h"
#include "PrlInputMappingContext.h"
#include "../PxcNativeLibrary.h"

void UPrlEnhancedInputConfig::ReadTo(UPrlInputMappingContext* pPrlIMC) const
{
	check(IsValid(pPrlIMC));

	for (const FPrlAxisMappingConfig& ActionConfig : m_tarrAxisMappings)
	{
		FEnhancedActionKeyMapping KeyMapping;
		//KeyMapping.Action = NewObject<UInputAction>();//FLAGJK_Now ËÑË÷LoadObject
	}
}

void UPrlEnhancedInputConfig::WriteFrom(const UPrlInputMappingContext* pPrlIMC)
{
	check(IsValid(pPrlIMC));
	m_tarrActionMappings.Empty();
	m_tarrAxisMappings.Empty();

	pPrlIMC->ForEachKeyMappings([this](const FEnhancedActionKeyMapping& KeyMapping) {
		if (KeyMapping.Action)
		{
			if (KeyMapping.Action->ValueType == EInputActionValueType::Boolean)
			{
				m_tarrActionMappings.Add(FPrlActionMappingConfig(KeyMapping.Action->GetFName(), KeyMapping.Key,
					FPxcNativeLibrary::EI_GetModifierCodeFromTriggers(KeyMapping.Triggers)));
			}
			else
			{
				m_tarrAxisMappings.Add(FPrlAxisMappingConfig(KeyMapping.Action->GetFName(), KeyMapping.Key,
					!FPxcNativeLibrary::EI_IsPositiveFromModifiers(KeyMapping.Modifiers)));
			}
		}
	});
}

void UPrlEnhancedInputConfig::SaveConfigMappings()
{
	m_tarrActionMappings.Sort();
	m_tarrAxisMappings.Sort();
	SaveConfig();
}