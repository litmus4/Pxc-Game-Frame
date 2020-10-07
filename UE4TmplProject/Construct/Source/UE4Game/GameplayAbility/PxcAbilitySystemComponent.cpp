// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAbilitySystemComponent.h"
#include "Framework/PxcGameConfig.h"
#include "Framework/PxcAssistantSubsystem.h"
#include "PxcUtil/IDPool64.h"

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

		TArray<FGameplayTag> tarrTags;
		CurEffect.Spec.Def->InheritableGameplayEffectTags.CombinedTags.GetGameplayTagArray(tarrTags);
		for (FGameplayTag& Tag : tarrTags)
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

float UPxcAbilitySystemComponent::ProcessFloatFigureByTagsTypeUid(const FGameplayTagContainer& Container, float fValue,
	EDynamicFigureType eType, int64 lUid, bool bRestart)
{
	float fNewValue = fValue;
	const FDynamicFigureFeature* pFeature = nullptr;
	if (eType != EDynamicFigureType::Unknown)
	{
		auto& tmap = GetDefault<UPxcGameConfig>()->tmapDynamicFigureToFeature;
		pFeature = tmap.Find(eType);
	}

	TArray<FGameplayTag> tarrTags;
	Container.GetGameplayTagArray(tarrTags);
	for (FGameplayTag& Tag : tarrTags)
	{
		//PassiveListener
		FPassiveListenerGrpList* pList = m_tmapPassiveListeners.Find(Tag);
		if (pList)
		{
			for (FPassiveListenerGroup& Group : pList->tarr)
			{
				if (pFeature && Group.eType != pFeature->ePassiveListeningType)
					continue;

				switch (Group.eType)
				{
				case EPassiveListeningType::AttackExtra:
				{
					if (ProcessOnePassiveGroup<FPassiveAttackExListener>(Group, lUid, bRestart,
						[&fNewValue, &fValue](FPassiveAttackExListener* pListener) {
							fNewValue = pListener->Process(fNewValue, fValue);
						})) return fNewValue;
					break;
				}
				}
			}
		}

		//Others...
	}
	return fNewValue;
}

void UPxcAbilitySystemComponent::AddPassiveListener(const FGameplayTag& Tag, const FSharedSignature& Sig, EPassiveListeningType eType)
{
	FPassiveListenerGrpList* pList = m_tmapPassiveListeners.Find(Tag);
	if (pList)
	{
		bool bFound = false;
		for (FPassiveListenerGroup& Group : pList->tarr)
		{
			if (Group.eType != eType) continue;
			bFound = true;
			Group.tsetSignatures.Add(Sig);
		}

		if (!bFound)
			pList->tarr.Add(FPassiveListenerGroup(Sig, eType));
	}
	else
	{
		FPassiveListenerGrpList List;
		List.tarr.Add(FPassiveListenerGroup(Sig, eType));
		m_tmapPassiveListeners.Add(Tag, List);
	}
}

void UPxcAbilitySystemComponent::ModifyPassiveListenerByTagUid(const FGameplayTag& Tag, int64 lUid, const FSharedSignature& ModiSig)
{
	FPassiveListenerGrpList* pList = m_tmapPassiveListeners.Find(Tag);
	if (!pList) return;

	FSharedSignature FakeSig = FSharedSignature(MakeShared<FPassiveListener>(lUid).Get());
	for (FPassiveListenerGroup& Group : pList->tarr)
	{
		switch (Group.eType)
		{
		case EPassiveListeningType::AttackExtra:
		{
			if (ProcessOnePassiveGroup<FPassiveAttackExListener>(Group, lUid, false, [&ModiSig](FPassiveAttackExListener* pListener) {
				check(ModiSig.IsDerived<FPassiveAttackExListener>());
				auto pModiListener = ModiSig.GetDerivedPtr<FPassiveAttackExListener>();
				pListener->fAttackAdd = pModiListener->fAttackAdd;
				pListener->fAttackMulti = pModiListener->fAttackMulti;
				})) return;
			break;
		}
		}
	}
}

void UPxcAbilitySystemComponent::RemovePassiveListenerByTagUid(const FGameplayTag& Tag, int64 lUid)
{
	FPassiveListenerGrpList* pList = m_tmapPassiveListeners.Find(Tag);
	if (!pList) return;

	if (lUid >= 0LL)
	{
		FSharedSignature FakeSig = FSharedSignature(MakeShared<FPassiveListener>(lUid).Get());
		for (FPassiveListenerGroup& Group : pList->tarr)
		{
			FSharedSignature* pSig = Group.tsetSignatures.Find(FakeSig);
			if (pSig)
			{
				PxcUtil::CIDPool64* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetUidPool64(ASUIDPOOL::PointerModelLL);
				pUidPool->Free((*pSig)->lUid);
				Group.tsetSignatures.Remove(*pSig);
				return;
			}
		}
	}
	else
		m_tmapPassiveListeners.Remove(Tag);
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