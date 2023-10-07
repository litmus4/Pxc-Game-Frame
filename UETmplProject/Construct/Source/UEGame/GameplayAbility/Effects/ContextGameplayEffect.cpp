// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayAbility/Effects/ContextGameplayEffect.h"
#include "AbilitySystemComponent.h"

void UContextGameplayEffect::GetGrantedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	TagContainer.AppendTags(GetGrantedTags());
	TagContainer.AppendTags(GetAssetTags());
}

void UContextGameplayEffect::AddContextAndHandle(UAbilitySystemComponent* pASC,
	const FGameplayEffectContextHandle& xContext, const FActiveGameplayEffectHandle& xHandle)
{
	check(pASC);
	tPairContext Pair;
	Pair.first = xContext;
	Pair.second = xHandle;
	m_mapContexts.insert(std::make_pair(pASC, Pair));
}

void UContextGameplayEffect::RemoveContextPair(UAbilitySystemComponent* pASC)
{
	m_mapContexts.erase(pASC);
}

AActor* UContextGameplayEffect::GetContextInstigator(UAbilitySystemComponent* pASC)
{
	tMapContext::iterator iter = m_mapContexts.find(pASC);
	if (iter != m_mapContexts.end())
		return iter->second.first.GetInstigator();
	return nullptr;
}

float UContextGameplayEffect::GetEffectLevel(UAbilitySystemComponent* pASC)
{
	tMapContext::iterator iter = m_mapContexts.find(pASC);
	if (pASC && iter != m_mapContexts.end() && iter->second.second.IsValid())
	{
		const FActiveGameplayEffect* pActiveEffect = pASC->GetActiveGameplayEffect(iter->second.second);
		if (pActiveEffect)
			return pActiveEffect->Spec.GetLevel();
	}
	return 0.0f;
}