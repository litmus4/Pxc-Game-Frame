// Fill out your copyright notice in the Description page of Project Settings.


#include "PrlInputMappingContext.h"
#include "EnhancedInputSubsystems.h"
#include "Framework/PxcNativeLibrary.h"
#include "PrlEnhancedInputConfig.h"

UPrlInputMappingContext::UPrlInputMappingContext()
{
	//
}

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

	FPxcNativeLibrary::EI_AddUniqueMapping(Mappings, KeyMapping);//Mappings.AddUnique(KeyMapping);
	if (bForceRebuildKeymaps)
	{
		check(IsValid(m_pSubsystem));
		m_pSubsystem->RequestRebuildControlMappings();
	}
}

void UPrlInputMappingContext::GetActionMappingByName(const FName ActionName, TArray<FEnhancedActionKeyMapping>& tarrOutMappings) const
{
	if (ActionName.IsValid())
	{
		for (int32 iIndex = Mappings.Num() - 1; iIndex >= 0; --iIndex)
		{
			if (!Mappings[iIndex].Action || Mappings[iIndex].Action->ValueType != EInputActionValueType::Boolean)
				continue;

			if (Mappings[iIndex].Action->GetFName() == ActionName)
			{
				tarrOutMappings.Add(Mappings[iIndex]);
				// we don't break because the mapping may have been in the array twice
			}
		}
	}
}

void UPrlInputMappingContext::RemoveActionMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps)
{
	if (!KeyMapping.Action || KeyMapping.Action->ValueType != EInputActionValueType::Boolean)
		return;

	for (int32 iIndex = Mappings.Num() - 1; iIndex >= 0; --iIndex)
	{
		if (FPxcNativeLibrary::EI_IsMappingEqual(Mappings[iIndex], KeyMapping)/*Mappings[iIndex] == KeyMapping*/)
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

void UPrlInputMappingContext::AddAxisMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps)
{
	if (!KeyMapping.Action || KeyMapping.Action->ValueType == EInputActionValueType::Boolean)
		return;

	FPxcNativeLibrary::EI_AddUniqueMapping(Mappings, KeyMapping);//Mappings.AddUnique(KeyMapping);
	if (bForceRebuildKeymaps)
	{
		check(IsValid(m_pSubsystem));
		m_pSubsystem->RequestRebuildControlMappings();
	}
}

void UPrlInputMappingContext::GetAxisMappingByName(const FName AxisName, TArray<FEnhancedActionKeyMapping>& tarrOutMappings) const
{
	if (AxisName.IsValid())
	{
		for (int32 iIndex = Mappings.Num() - 1; iIndex >= 0; --iIndex)
		{
			if (!Mappings[iIndex].Action || Mappings[iIndex].Action->ValueType == EInputActionValueType::Boolean)
				continue;

			if (Mappings[iIndex].Action->GetFName() == AxisName)
			{
				tarrOutMappings.Add(Mappings[iIndex]);
				// we don't break because the mapping may have been in the array twice
			}
		}
	}
}

void UPrlInputMappingContext::RemoveAxisMapping(const FEnhancedActionKeyMapping& InKeyMapping, bool bForceRebuildKeymaps)
{
	if (!InKeyMapping.Action || InKeyMapping.Action->ValueType == EInputActionValueType::Boolean)
		return;

	for (int32 iIndex = Mappings.Num() - 1; iIndex >= 0; --iIndex)
	{
		FEnhancedActionKeyMapping& KeyMapping = Mappings[iIndex];
		if (KeyMapping.Action == InKeyMapping.Action
			&& KeyMapping.Key == InKeyMapping.Key)
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

bool UPrlInputMappingContext::LoadKeyMappings()
{
	if (!IsValid(m_pConfig))
	{
		m_pConfig = GetMutableDefault<UPrlEnhancedInputConfig>();
		check(m_pConfig);
		if (m_pConfig->IsValidConfig())
		{
			Mappings.Empty();
			m_pConfig->ReadTo(this);
			return true;
		}
	}
	return (IsValid(m_pConfig) && m_pConfig->IsValidConfig());
}

void UPrlInputMappingContext::SaveKeyMappings()
{
	if (IsValid(m_pConfig))
	{
		m_pConfig->WriteFrom(this);
        m_pConfig->SaveConfigMappings();
	}
}

void UPrlInputMappingContext::AddMappingQuickly(const FEnhancedActionKeyMapping& KeyMapping)
{
	Mappings.Add(KeyMapping);
}

void UPrlInputMappingContext::ForEachKeyMappings(std::function<void(const FEnhancedActionKeyMapping&)>&& fnOnEach) const
{
	for (int32 i = 0; i < Mappings.Num(); ++i)
		fnOnEach(Mappings[i]);
}

//*²âÊÔÁÙÊ±
void UPrlInputMappingContext::TestMappingEqual(const FEnhancedActionKeyMapping& KeyMapping3)
{
	if (Mappings.Num() >= 2)
	{
		FEnhancedActionKeyMapping& KeyMapping1 = Mappings[0];
		FEnhancedActionKeyMapping& KeyMapping2 = Mappings[1];
		bool bTest = (KeyMapping1 == KeyMapping1);
		bool bTest4 = (KeyMapping1 == KeyMapping3);
		bool bTest5 = FPxcNativeLibrary::EI_IsMappingEqual(KeyMapping1, KeyMapping3);
		if (KeyMapping1.Triggers.Num() > 0 && KeyMapping2.Triggers.Num() > 0)
		{
			bool bTest2 = (KeyMapping1.Triggers[0] == KeyMapping2.Triggers[0]);
			bool bTest3 = (KeyMapping1.Triggers == KeyMapping2.Triggers);
			int32 iii = 0;
		}
	}
}
//*/