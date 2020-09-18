// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAttributeSet.h"
#include "GameplayEffect.h"
#include "../PxcAbilitySystemGlobals.h"

void UPxcAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& fNewValue)
{
	UAbilitySystemComponent* pASC = GetOwningAbilitySystemComponent();
	check(pASC);

	float fDiff = 0.0f, fCurValue = 0.0f;
	if (CanAttrbuteReviseByGE(Attribute, &fCurValue))
	{
		fDiff = fNewValue - fCurValue;
		TryTriggerPreGEContModify(Attribute, fNewValue, fDiff);
	}
	PreRevisedAttributeChange(Attribute, fNewValue, fDiff);

	//TODOJK Í¨ÖªPawn
}

void UPxcAttributeSet::PreGameplayEffectContModify(const UGameplayEffect* pEffect, int32 iModIndex, float& fNewValue, float& fDiff)
{
	//
}

void UPxcAttributeSet::PreRevisedAttributeChange(const FGameplayAttribute& Attribute, float& NewValue, float fDiff)
{
	//
}

//Ä£°å
bool UPxcAttributeSet::CanAttrbuteReviseByGE(const FGameplayAttribute& Attribute, float* pOutValue)
{
	static const TMap<FGameplayAttribute, float> tmapCheck = {
		//CANREVISE_PAIR
	};

	const float* pValue = tmapCheck.Find(Attribute);
	if (pValue)
	{
		if (pOutValue) *pOutValue = *pValue;
		return true;
	}
	return false;
}

void UPxcAttributeSet::TryTriggerPreGEContModify(const FGameplayAttribute& Attribute, float& fNewValue, float& fDiff)
{
	UPxcAbilitySystemGlobals* pASG = Cast<UPxcAbilitySystemGlobals>(&UAbilitySystemGlobals::Get());
	if (pASG && pASG->m_pNowAppliedGE && pASG->m_pNowAppliedGE->DurationPolicy != EGameplayEffectDurationType::Instant)
	{
		int32 iModIndex = -1;
		for (int32 i = 0; i < pASG->m_pNowAppliedGE->Modifiers.Num(); ++i)
		{
			const FGameplayModifierInfo& Modifier = pASG->m_pNowAppliedGE->Modifiers[i];
			if (Modifier.Attribute != Attribute)
				continue;

			float fMagnitude = 0.0f;
			if (!Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(1.0f, fMagnitude))
				continue;

			float fCheckMagnitude = 0.0f;
			switch (Modifier.ModifierOp)
			{
			case EGameplayModOp::Type::Additive:
				fCheckMagnitude = fDiff; break;
			case EGameplayModOp::Type::Multiplicitive:
				check(FMath::Abs(fNewValue - fDiff - 0.0f) > 0.00001f);
				fCheckMagnitude = fNewValue / (fNewValue - fDiff); break;
			case EGameplayModOp::Type::Division:
				check(FMath::Abs(fNewValue - 0.0f) > 0.00001f);
				fCheckMagnitude = (fNewValue - fDiff) / fNewValue; break;
			case EGameplayModOp::Type::Override:
				fCheckMagnitude = fNewValue; break;
			}

			if (FMath::Abs(fCheckMagnitude - fMagnitude) < 0.00001f)
			{
				iModIndex = i;
				break;
			}
		}
		if (iModIndex >= 0)
			PreGameplayEffectContModify(pASG->m_pNowAppliedGE, iModIndex, fNewValue, fDiff);
	}
}