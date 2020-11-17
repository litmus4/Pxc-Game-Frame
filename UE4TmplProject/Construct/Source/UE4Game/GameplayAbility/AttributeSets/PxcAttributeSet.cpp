// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAttributeSet.h"
#include "GameplayEffect.h"
#include "../PxcAbilitySystemGlobals.h"

void UPxcAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& fNewValue)
{
	UAbilitySystemComponent* pASC = GetOwningAbilitySystemComponent();
	check(pASC);

	float fDiff = 0.0f, fCurValue = 0.0f;
	if (CanAttrbuteRevisedByGE(Attribute, &fCurValue))
	{
		fDiff = fNewValue - fCurValue;
		TryTriggerPreGEContModify(Attribute, fNewValue, fDiff);
	}
	PreRevisedAttributeChange(Attribute, fNewValue, fDiff);

	//TODOJK 通知Pawn
	//在ASC的OnGameplayEffectAppliedDelegateToSelf回调内部调用UPxcAbilitySystemGlobals::RemoveGEContModify
}

void UPxcAttributeSet::PreGameplayEffectContModify(const UGameplayEffect* pEffect, int32 iModIndex, float& fNewValue, float& fDiff)
{
	//
}

void UPxcAttributeSet::PreRevisedAttributeChange(const FGameplayAttribute& Attribute, float& NewValue, float fDiff)
{
	//
}

//模板
bool UPxcAttributeSet::CanAttrbuteRevisedByGE(const FGameplayAttribute& Attribute, float* pOutValue)
{
	static const TMap<FGameplayAttribute, float> tmapCheck = {
		//CANREVISE_PAIR//FLAGJK 要保存函数指针，下边再改为调用
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
	if (!pASG) return;

	UAbilitySystemComponent* pASC = GetOwningAbilitySystemComponent();
	const UGameplayEffect* pContModEffect = nullptr;
	int32 iModIndex = pASG->FindGEContModifyFromQueue(pASC, Attribute,
		[fNewValue, fDiff](const FGameplayModifierInfo& Modifier)->bool {
			float fMagnitude = 0.0f;
			if (!Modifier.ModifierMagnitude.GetStaticMagnitudeIfPossible(1.0f, fMagnitude))
				return false;

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
				return true;
			return false;
		},
	&pContModEffect);

	if (iModIndex >= 0)
		PreGameplayEffectContModify(pContModEffect, iModIndex, fNewValue, fDiff);
}