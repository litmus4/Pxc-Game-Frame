// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAbilitySystemComponent.h"

int32 UPxcAbilitySystemComponent::RemoveActiveEffects(const FGameplayEffectQuery& Query, int32 iStacksToRemove)
{
	TArray<FActiveGameplayEffectHandle>&& ActiveHandles = GetActiveEffects(Query);
	for (FActiveGameplayEffectHandle& Handle : ActiveHandles)
	{
		const FActiveGameplayEffect* pActiveEffect = GetActiveGameplayEffect(Handle);
		int32 iStackCount = GetCurrentStackCount(Handle);
		if (pActiveEffect && iStackCount > 1)
			OnAnyGameplayEffectRemovedDelegate().Broadcast(*pActiveEffect);
	}

	return Super::RemoveActiveEffects(Query, iStacksToRemove);
}

void UPxcAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	K2_OnGiveAbility(AbilitySpec);
	DeleAbilityGiven.Broadcast(AbilitySpec);
}

void UPxcAbilitySystemComponent::OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnRemoveAbility(AbilitySpec);

	K2_OnRemoveAbility(AbilitySpec);
	DeleAbilityRemoved.Broadcast(AbilitySpec);
}