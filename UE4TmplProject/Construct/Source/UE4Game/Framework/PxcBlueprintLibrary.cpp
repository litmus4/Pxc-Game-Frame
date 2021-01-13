// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcBlueprintLibrary.h"
#include "GameplayAbility/PxcAbilitySystemComponent.h"
#include "GameplayAbility/ExecutionCalculations/ExtentionExecution.h"

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