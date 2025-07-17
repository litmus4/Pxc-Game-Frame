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

void UPrlInputMappingContext::GetActionMappingByName(const FName ActionName, TArray<FEnhancedActionKeyMapping>& OutMappings) const
{
	if (ActionName.IsValid())
	{
		for (int32 iIndex = Mappings.Num() - 1; iIndex >= 0; --iIndex)
		{
			if (!Mappings[iIndex].Action || Mappings[iIndex].Action->ValueType != EInputActionValueType::Boolean)
				continue;

			if (Mappings[iIndex].Action->GetName() == ActionName)
			{
				OutMappings.Add(Mappings[iIndex]);
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

void UPrlInputMappingContext::AddAxisMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps)
{
	if (!KeyMapping.Action || KeyMapping.Action->ValueType == EInputActionValueType::Boolean)
		return;

	Mappings.AddUnique(KeyMapping);
	if (bForceRebuildKeymaps)
	{
		check(IsValid(m_pSubsystem));
		m_pSubsystem->RequestRebuildControlMappings();
	}
}

void UPrlInputMappingContext::GetAxisMappingByName(const FName AxisName, TArray<FEnhancedActionKeyMapping>& OutMappings) const
{
	if (AxisName.IsValid())
	{
		for (int32 iIndex = Mappings.Num() - 1; iIndex >= 0; --iIndex)
		{
			if (!Mappings[iIndex].Action || Mappings[iIndex].Action->ValueType == EInputActionValueType::Boolean)
				continue;

			if (Mappings[iIndex].Action->GetName() == AxisName)
			{
				OutMappings.Add(Mappings[iIndex]);
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

//*测试临时
void UPrlInputMappingContext::TestMappingEqual(const FEnhancedActionKeyMapping& KeyMapping3)
{
	if (Mappings.Num() >= 2)
	{
		FEnhancedActionKeyMapping& KeyMapping1 = Mappings[0];
		FEnhancedActionKeyMapping& KeyMapping2 = Mappings[1];
		bool bTest = (KeyMapping1 == KeyMapping1);
		bool bTest4 = (KeyMapping1 == KeyMapping3);//FLAGJK_Now 等于测试失败，自己重写等于函数吧
		if (KeyMapping1.Triggers.Num() > 0 && KeyMapping2.Triggers.Num() > 0)
		{
			bool bTest2 = (KeyMapping1.Triggers[0] == KeyMapping2.Triggers[0]);
			bool bTest3 = (KeyMapping1.Triggers == KeyMapping2.Triggers);
			int32 iii = 0;
		}
	}
}
//*/