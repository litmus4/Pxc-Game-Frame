// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcBlueprintLibrary.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/KismetInputLibrary.h"
#include "GameplayAbility/PxcAbilitySystemComponent.h"
#include "GameplayAbility/ExecutionCalculations/ExtentionExecution.h"
#include "PXCycleInstance.h"
#include "PxcGameConfig.h"

DEFINE_LOG_CATEGORY(LogPxcBPLib);

FActiveGameplayEffectHandle UPxcBlueprintLibrary::ApplyExtentionGESpecWithReturn(UAbilitySystemComponent* pASC, const FGameplayEffectSpecHandle& SpecHandle,
	FActiveGameplayEffectHandle StackedHandle, TMap<FString, float>& tmapRet)
{
	check(pASC);
	auto Handle = pASC->BP_ApplyGameplayEffectSpecToSelf(SpecHandle);
	UPxcAbilitySystemComponent* pPASC = Cast<UPxcAbilitySystemComponent>(pASC);
	if (pPASC)
	{
		if (Handle.WasSuccessfullyApplied())
		{
			pPASC->ForEachGEExtentionReturnToStack(StackedHandle.IsValid() ? StackedHandle : Handle,
				[&SpecHandle, &tmapRet](const std::string& strName, float fValue) {
					SpecHandle.Data->SetSetByCallerMagnitude(FName(strName.c_str()), fValue);
					tmapRet.Add(FString(strName.c_str()), fValue);
				}
			);
		}
		else
		{
			std::map<std::string, float>::iterator iter = pPASC->m_mapGEExtentionReturn.begin();
			for (; iter != pPASC->m_mapGEExtentionReturn.end(); iter++)
			{
				SpecHandle.Data->SetSetByCallerMagnitude(FName(iter->first.c_str()), iter->second);
				tmapRet.Add(FString(iter->first.c_str()), iter->second);
			}
			pPASC->m_mapGEExtentionReturn.clear();
		}
	}
	return Handle;
}

void UPxcBlueprintLibrary::DiscardExtentionGEByStackedHandle(UAbilitySystemComponent* pASC, FActiveGameplayEffectHandle StackedHandle)
{
	check(pASC);
	const FActiveGameplayEffect* pActiveEffect = pASC->GetActiveGameplayEffect(StackedHandle);
	if (pActiveEffect)
		UExtentionExecution::Discard(pASC, pActiveEffect->Spec, pActiveEffect->Handle);
}

int32 UPxcBlueprintLibrary::PushIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, float fValue)
{
	return Stack.PushValue(fValue);
}

void UPxcBlueprintLibrary::PopIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, int32 iUid)
{
	Stack.PopValue(iUid);
}

int32 UPxcBlueprintLibrary::PushIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, float fValue)
{
	return Stack.PushValue(fValue);
}

void UPxcBlueprintLibrary::PopIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, int32 iUid)
{
	Stack.PopValue(iUid);
}

FName UPxcBlueprintLibrary::Key_GetName(const FKey& Key)
{
	return Key.GetFName();
}

void UPxcBlueprintLibrary::Key_GetAllActionMappingByName(const FName& ActionName,
	TArray<FInputActionKeyMapping>& tarrOutMappings, TArray<FGamepadCombineMapping>& tarrOutGamepadCombMappings)
{
	const UInputSettings* pSetting = GetDefault<UInputSettings>();
	pSetting->GetActionMappingByName(ActionName, tarrOutMappings);

	auto& tmapGamepadCombineMappings = GetDefault<UPxcGameConfig>()->GetGamepadCombineMappings();
	const TArray<FGamepadCombineMapping>* pCombMappingList = tmapGamepadCombineMappings.Find(ActionName);
	if (pCombMappingList)
	{
		for (const FGamepadCombineMapping& CombineMapping : *pCombMappingList)
			tarrOutGamepadCombMappings.Add(CombineMapping);
	}
}

int32 UPxcBlueprintLibrary::Key_GetFirstAdaptiveAcMapping(const TArray<FInputActionKeyMapping>& tarrMappings, const TArray<FGamepadCombineMapping>& tarrGamepadCombMappings,
	FInputActionKeyMapping& OutMapping, FGamepadCombineMapping& OutGamepadCombMapping)
{
	UPXCycleInstance* pGI = Cast<UPXCycleInstance>(GWorld ? GWorld->GetGameInstance() : nullptr);
	if (!pGI) return 0;

	for (const FInputActionKeyMapping& Mapping : tarrMappings)
	{
		bool bCurKb = UKismetInputLibrary::Key_IsKeyboardKey(Mapping.Key);
		auto pComb = tarrGamepadCombMappings.FindByPredicate([&Mapping](const FGamepadCombineMapping& CurCM)->bool {
			return CurCM.MainKey == Mapping.Key;
		});
		if (pGI->bKeyboardRuntime ? bCurKb : (!bCurKb && !pComb))
		{
			OutMapping = Mapping;
			return 1;
		}
	}
	if (pGI->bKeyboardRuntime) return 0;

	for (const FGamepadCombineMapping& CombineMapping : tarrGamepadCombMappings)
	{
		if (CombineMapping.MainKey.IsGamepadKey())
		{
			OutGamepadCombMapping = CombineMapping;
			return -1;
		}
	}

	return 0;
}

int32 UPxcBlueprintLibrary::Key_GetIndexedDevisedAcMapping(const TArray<FInputActionKeyMapping>& tarrMappings, const TArray<FGamepadCombineMapping>& tarrGamepadCombMappings,
	bool bDevisedKeyboard, int32 iIndex, bool bGamepadCombBoth, FInputActionKeyMapping& OutMapping, FGamepadCombineMapping& OutGamepadCombMapping)
{
	int32 i = 0;

	bool bBothFound = false;
	for (const FInputActionKeyMapping& Mapping : tarrMappings)
	{
		bool bCurKb = UKismetInputLibrary::Key_IsKeyboardKey(Mapping.Key);
		int32 iCombIndex = tarrGamepadCombMappings.IndexOfByPredicate([&Mapping](const FGamepadCombineMapping& CurCM)->bool {
			return CurCM.MainKey == Mapping.Key;
		});
		if (bDevisedKeyboard ? bCurKb : (!bCurKb && (bGamepadCombBoth || iCombIndex == INDEX_NONE)))
		{
			if (i == iIndex)
			{
				OutMapping = Mapping;
				if (iCombIndex != INDEX_NONE)//Both
				{
					iIndex = tarrMappings.Num() + iCombIndex;
					bBothFound = true;
					i++; break;
				}
				else return 1;
			}
			i++;
		}
	}
	if (bDevisedKeyboard) return 0;
	if (bGamepadCombBoth && !bBothFound) return 0;

	for (const FGamepadCombineMapping& CombineMapping : tarrGamepadCombMappings)
	{
		if (CombineMapping.MainKey.IsGamepadKey())
		{
			if (i == iIndex)
			{
				OutGamepadCombMapping = CombineMapping;
				return -1;
			}
			i++;
		}
	}

	return 0;
}