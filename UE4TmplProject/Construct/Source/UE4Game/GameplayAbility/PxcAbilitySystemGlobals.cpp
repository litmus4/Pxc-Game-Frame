// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayTagResponseTable.h"

DEFINE_LOG_CATEGORY(LogPxcGAS);

void UPxcAbilitySystemGlobals::InitGlobalData()
{
	Super::InitGlobalData();

	for (UCurveTable* pTable : GlobalAttributeDefaultsTables)
	{
		if (pTable)
			pTable->AddToRoot();
	}

	if (GlobalGameplayCueManager)
		GlobalGameplayCueManager->AddToRoot();

	if (GameplayTagResponseTable)
		GameplayTagResponseTable->AddToRoot();
}

void UPxcAbilitySystemGlobals::SetCurrentAppliedGE(const FGameplayEffectSpec* pSpec)
{
	if (!pSpec->Def || pSpec->Def->DurationPolicy == EGameplayEffectDurationType::Instant)
		return;
	check(pSpec->GetContext().IsValid());

	FGameplayTag ContModTag = FGameplayTag::RequestGameplayTag(TEXT("GEAsset.ContinuousModify"));
	if (!pSpec->Def->InheritableGameplayEffectTags.CombinedTags.HasTag(ContModTag))
		return;

	FGEContModifyInfo Info;
	Info.pASC = pSpec->GetContext().GetInstigatorAbilitySystemComponent();
	Info.pEffect = pSpec->Def;

	for (int32 i = 0; i < pSpec->Def->Modifiers.Num(); ++i)
	{
		const FGameplayModifierInfo& Modifier = pSpec->Def->Modifiers[i];
		if (Modifier.ModifierMagnitude.GetMagnitudeCalculationType() ==
			EGameplayEffectMagnitudeCalculation::ScalableFloat)
		{
			Info.tmapAttrToIndex.Add(Modifier.Attribute, i);
		}
	}

	if (Info.tmapAttrToIndex.Num() > 0)
		m_lisGEContModifyQueue.push_back(Info);
}

int32 UPxcAbilitySystemGlobals::FindGEContModifyFromQueue(UAbilitySystemComponent* pASC, const FGameplayAttribute& Attribute,
	std::function<bool(const FGameplayModifierInfo&)>&& fnQuery, const UGameplayEffect** ppOutEffect)
{
	std::list<FGEContModifyInfo>::iterator iter = m_lisGEContModifyQueue.begin();
	for (;iter != m_lisGEContModifyQueue.end(); iter++)
	{
		if (iter->pASC != pASC) continue;

		int32* pIndex = iter->tmapAttrToIndex.Find(Attribute);
		if (!pIndex) continue;
		int32 iIndex = *pIndex;
		
		if (fnQuery(iter->pEffect->Modifiers[iIndex]))
		{
			if (ppOutEffect) *ppOutEffect = iter->pEffect;
			iter->tmapAttrToIndex.Remove(Attribute);
			if (iter->tmapAttrToIndex.Num() == 0)
				m_lisGEContModifyQueue.erase(iter);
			return iIndex;
		}
	}
	return -1;
}

void UPxcAbilitySystemGlobals::RemoveGEContModify(UAbilitySystemComponent* pASC, const UGameplayEffect* pEffect)
{
	std::list<FGEContModifyInfo>::iterator iter = m_lisGEContModifyQueue.begin();
	for (; iter != m_lisGEContModifyQueue.end(); iter++)
	{
		if (iter->pASC == pASC && iter->pEffect == pEffect)
		{
			m_lisGEContModifyQueue.erase(iter);
			return;
		}
	}
}

void UPxcAbilitySystemGlobals::ClearRedundantGEContModify(bool bWarn)
{
	if (!m_lisGEContModifyQueue.empty())
	{
		if (bWarn)
		{
			UE_LOG(LogPxcGAS, Warning, TEXT("ClearRedundantGEContModify: Redundant!"));
			//TODOJK  π”√LogCenter
		}
		m_lisGEContModifyQueue.clear();
	}
}