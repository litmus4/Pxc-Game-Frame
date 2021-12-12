// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/ExecutionCalculations/ExtentionExecution.h"
#include "GameplayAbility/PxcAbilitySystemComponent.h"
#include "PxcUtil/StringTools.h"

UExtentionExecution::UExtentionExecution(const FObjectInitializer& ObjectInitializer)
{
	bRequiresPassedInTags = true;
	ExtentionVariableTag1 = FGameplayTag::RequestGameplayTag(TEXT("ExecutionVariable.Extention.Variable1"));
	ExtentionVariableTag2 = FGameplayTag::RequestGameplayTag(TEXT("ExecutionVariable.Extention.Variable2"));
	ExtentionVariableTag3 = FGameplayTag::RequestGameplayTag(TEXT("ExecutionVariable.Extention.Variable3"));

#if WITH_EDITORONLY_DATA
	ValidTransientAggregatorIdentifiers.AddTag(ExtentionVariableTag1);
	ValidTransientAggregatorIdentifiers.AddTag(ExtentionVariableTag2);
	ValidTransientAggregatorIdentifiers.AddTag(ExtentionVariableTag3);
#endif
}

void UExtentionExecution::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	FGameplayEffectSpec* pSpec = ExecutionParams.GetOwningSpecForPreExecuteMod();
	UPxcAbilitySystemComponent* pTargetASC = Cast<UPxcAbilitySystemComponent>(ExecutionParams.GetTargetAbilitySystemComponent());
	check(pTargetASC)

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = pSpec->CapturedSourceTags.GetAggregatedTags();
	EvaluateParameters.TargetTags = pSpec->CapturedTargetTags.GetAggregatedTags();

	float fVar1 = 0.0f;
	ExecutionParams.AttemptCalculateTransientAggregatorMagnitude(ExtentionVariableTag1, EvaluateParameters, fVar1);

	float fVar2 = 0.0f;
	ExecutionParams.AttemptCalculateTransientAggregatorMagnitude(ExtentionVariableTag2, EvaluateParameters, fVar2);

	float fVar3 = 0.0f;
	ExecutionParams.AttemptCalculateTransientAggregatorMagnitude(ExtentionVariableTag3, EvaluateParameters, fVar3);

	float fVars[3] = { fVar1, fVar2, fVar3 };
	for (int32 i = 0; i < 3; ++i)
	{
		EGEExtentionType eType = (EGEExtentionType)((int32)fVars[i]);
		if (eType != EGEExtentionType::Unknown)
			pTargetASC->m_mapGEExtentionReturn.insert(std::make_pair(PxcUtil::StringTools::Format("ExType%d", i), fVars[i]));

		TMap<FString, float> tmapNames;
		pTargetASC->ExecuteGEExtentionByType(eType, fVars[i], tmapNames);
		for (auto& Pair : tmapNames)
		{
			std::string&& strName = TCHAR_TO_ANSI(*Pair.Key);
			pTargetASC->m_mapGEExtentionReturn.insert(std::make_pair(strName, Pair.Value));
		}
	}

	OutExecutionOutput.MarkConditionalGameplayEffectsToTrigger();
}

void UExtentionExecution::Discard(UAbilitySystemComponent* pASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	UPxcAbilitySystemComponent* pPASC = Cast<UPxcAbilitySystemComponent>(pASC);
	check(pPASC);
	TMap<FString, float> tmapFakeSpec;
	pPASC->PopGEExtentionStackByHandle(Handle, tmapFakeSpec);
	check(tmapFakeSpec.Num() > 0);

	for (int32 i = 0; i < 3; ++i)
	{
		float* pfType = tmapFakeSpec.Find(FString::Printf(TEXT("ExType%d"), i));
		EGEExtentionType eType = (EGEExtentionType)(pfType ? (int32)*pfType : 0);
		pPASC->DiscardGEExtentionByType(eType, tmapFakeSpec);
	}
}