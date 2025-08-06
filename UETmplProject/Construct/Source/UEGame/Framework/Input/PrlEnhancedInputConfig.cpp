// Fill out your copyright notice in the Description page of Project Settings.


#include "PrlEnhancedInputConfig.h"
#include "PrlInputMappingContext.h"
#include "../PxcNativeLibrary.h"
#include "../PxcGameConfig.h"
#include "PublicDefinitions/AssetsDef.h"

void UPrlEnhancedInputConfig::ReadTo(UPrlInputMappingContext* pPrlIMC) const
{
	check(IsValid(pPrlIMC));
	const FString* pInputPath = GetDefault<UPxcGameConfig>()->tmapDynamicAssetsPathes.Find(EDynamicAssetsType::Input);
	check(pInputPath);

	for (const FPrlAxisMappingConfig& AxisConfig : m_tarrAxisMappings)
	{
		FEnhancedActionKeyMapping KeyMapping;

		FString&& sAxisName = AxisConfig.AxisName.ToString();
		FString&& sActionPath = FString::Printf(TEXT(UASSETREF_OBJECT), TEXT(UASSETREFHEAD_IA), **pInputPath, *sAxisName, *sAxisName);
		KeyMapping.Action = LoadObject<UInputAction>(nullptr, *sActionPath);

		KeyMapping.Key = AxisConfig.Key;

		FPxcNativeLibrary::EI_GetModifiersFromPositive(!AxisConfig.bNegative, KeyMapping.Modifiers);//FLAGJK_EI 完善增强输入New出来的Trigger和Modifier对象.png

		pPrlIMC->AddMappingQuickly(KeyMapping);
	}

	for (const FPrlActionMappingConfig& ActionConfig : m_tarrActionMappings)
	{
		FEnhancedActionKeyMapping KeyMapping;

		FString&& sActionName = ActionConfig.ActionName.ToString();
		FString&& sActionPath = FString::Printf(TEXT(UASSETREF_OBJECT), TEXT(UASSETREFHEAD_IA), **pInputPath, *sActionName, *sActionName);
		KeyMapping.Action = LoadObject<UInputAction>(nullptr, *sActionPath);

        KeyMapping.Key = ActionConfig.Key;

		FPxcNativeLibrary::EI_GetTriggersFromModifierCode(ActionConfig.uChordCode, KeyMapping.Triggers);

		pPrlIMC->AddMappingQuickly(KeyMapping);
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

bool UPrlEnhancedInputConfig::IsValidConfig() const
{
	return (!m_tarrActionMappings.IsEmpty() || !m_tarrAxisMappings.IsEmpty());
}

void UPrlEnhancedInputConfig::LoadConfigMappings(const FString& sConfigPath)
{
	if (sConfigPath != m_sConfigPath)
		m_sConfigPath = sConfigPath;

	if (!m_sConfigPath.IsEmpty())
	{
		GConfig->Flush(true, m_sConfigPath);
		LoadConfig(ThisClass::StaticClass(), *m_sConfigPath);
	}
}

void UPrlEnhancedInputConfig::SaveConfigMappings()
{
	m_tarrActionMappings.Sort();
	m_tarrAxisMappings.Sort();
	if (!m_sConfigPath.IsEmpty())
	{
		SaveConfig(CPF_Config, *m_sConfigPath);
		GConfig->Flush(false, m_sConfigPath);
	}
}