// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAbilitySystemComponent.h"
#include "Framework/PxcGameConfig.h"

int32 UPxcAbilitySystemComponent::RemoveActiveEffects(const FGameplayEffectQuery& Query, int32 iStacksToRemove)
{
	TArray<FActiveGameplayEffectHandle>&& tarrHandles = GetActiveEffects(Query);
	for (FActiveGameplayEffectHandle& Handle : tarrHandles)
	{
		const FActiveGameplayEffect* pActiveEffect = GetActiveGameplayEffect(Handle);
		int32 iStackCount = GetCurrentStackCount(Handle);
		if (pActiveEffect && iStackCount > 1)
			OnAnyGameplayEffectRemovedDelegate().Broadcast(*pActiveEffect);
	}

	return Super::RemoveActiveEffects(Query, iStacksToRemove);
}

TArray<FActiveGameplayEffectHandle> UPxcAbilitySystemComponent::GetActiveEffectsWithAnyTags(FGameplayTagContainer Tags) const
{
	return GetActiveEffects(FGameplayEffectQuery::MakeQuery_MatchAnyEffectTags(Tags));
}

TArray<FActiveGameplayEffectHandle> UPxcAbilitySystemComponent::GetActiveEffectsWithParentTag(
	FGameplayTag ParentTag, TArray<FGameplayTag>& tarrOutTags) const
{
	FGameplayEffectQuery Query;
	Query.CustomMatchDelegate.BindLambda([&ParentTag, &tarrOutTags](const FActiveGameplayEffect& CurEffect)->bool {
		if (!CurEffect.Spec.Def) return false;

		TArray<FGameplayTag> Tags;
		CurEffect.Spec.Def->InheritableGameplayEffectTags.CombinedTags.GetGameplayTagArray(Tags);
		for (FGameplayTag& Tag : Tags)
		{
			if (Tag.MatchesTag(ParentTag) && Tag != ParentTag)
			{
				tarrOutTags.Add(Tag);
				return true;
			}
		}
		return false;
	});
	return GetActiveEffects(Query);
}

UGameplayEffect* UPxcAbilitySystemComponent::K2_GetGameplayEffectDefForHandle(const FActiveGameplayEffectHandle& Handle)
{
	return const_cast<UGameplayEffect*>(GetGameplayEffectDefForHandle(Handle));
}

void UPxcAbilitySystemComponent::UpdateUnlockTags()
{
	m_tsetUnlockTags.Empty();

	TArray<FGameplayTag> tarrParentTags;
	GetDefault<UPxcGameConfig>()->UnlockGameplayEffectParentTags.GetGameplayTagArray(tarrParentTags);
	for (FGameplayTag& ParentTag : tarrParentTags)
	{
		TArray<FGameplayTag> tarrChildTags;
		GetActiveEffectsWithParentTag(ParentTag, tarrChildTags);
		m_tsetUnlockTags.Append(tarrChildTags);
	}
}

bool UPxcAbilitySystemComponent::IsUnlocked(const FGameplayTag& Tag)
{
	return m_tsetUnlockTags.Contains(Tag);
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