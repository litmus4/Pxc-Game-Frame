// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PrivateDefinitions/GameplayAbilityDef.h"
#include "Abilities/PassiveListenerStructs.h"
#include "PxcAbilitySystemComponent.generated.h"

USTRUCT()
struct FPassiveListenerGrpList
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FPassiveListenerGroup> tarr;
};

/**
 * 
 */
UCLASS(Blueprintable)
class UE4GAME_API UPxcAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityGivenDelegate, const FGameplayAbilitySpec&, AbilitySpec);
	
public:
	virtual int32 RemoveActiveEffects(const FGameplayEffectQuery& Query, int32 iStacksToRemove = -1);

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = GameplayEffects)
	TArray<FActiveGameplayEffectHandle> GetActiveEffectsWithAnyTags(FGameplayTagContainer Tags) const;

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = GameplayEffects)
	TArray<FActiveGameplayEffectHandle> GetActiveEffectsWithParentTag(FGameplayTag ParentTag, TArray<FGameplayTag>& tarrOutTags) const;

	UFUNCTION(BlueprintPure, Category = GameplayEffects)
	UGameplayEffect* K2_GetGameplayEffectDefForHandle(const FActiveGameplayEffectHandle& Handle);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	float ProcessFloatFigureByTagsTypeUid(const FGameplayTagContainer& Container, float fValue,
		EDynamicFigureType eType = EDynamicFigureType::Unknown, int64 lUid = -1, bool bRestart = false);

	void AddPassiveListener(const FGameplayTag& Tag, const FSharedSignature& Sig, EPassiveListeningType eType);

	UFUNCTION(BlueprintCallable, Category = GameplayAbility)
	void ModifyPassiveListenerByTagUid(const FGameplayTag& Tag, int64 lUid, const FSharedSignature& ModiSig);

	UFUNCTION(BlueprintCallable = Category = GameplayAbility)
	void RemovePassiveListenerByTagUid(const FGameplayTag& Tag, int64 lUid = -1);

	//GEExtention FLAGJK

	//TODOJK 在ASC的OnGameplayEffectAppliedDelegateToSelf回调和OnAnyGameplayEffectRemovedDelegate()回调内部调用
	void UpdateUnlockTags();

	UFUNCTION(BlueprintPure, Category = Gameplay)
	bool IsUnlocked(const FGameplayTag& Tag);

	UPROPERTY(BlueprintAssignable)
	FAbilityGivenDelegate DeleAbilityGiven;

	UPROPERTY(BlueprintAssignable)
	FAbilityGivenDelegate DeleAbilityRemoved;

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnGiveAbility(const FGameplayAbilitySpec& AbilitySpec);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnRemoveAbility(const FGameplayAbilitySpec& AbilitySpec);

	template<class T>
	bool ProcessOnePassiveGroup(FPassiveListenerGroup& Group, int64 lUid, bool bRestart, std::function<void(T*)> fnProc)
	{
		if (lUid >= 0LL)
		{
			FSharedSignature FakeSig = FSharedSignature(MakeShared<FPassiveListener>(lUid).Get());
			FSharedSignature* pSig = Group.tsetSignatures.Find(FakeSig);
			if (pSig)
			{
				if (pSig->IsDerived<T>())
				{
					T* pListener = pSig->GetDerivedPtr<T>();
					if (bRestart) pListener->Restart();
					fnProc(pListener);
				}
				return true;
			}
		}
		else
		{
			for (FSharedSignature& Sig : Group.tsetSignatures)
			{
				if (Sig.IsDerived<T>())
				{
					T* pListener = Sig.GetDerivedPtr<T>();
					if (bRestart) pListener->Restart();
					fnProc(pListener);
				}
			}
		}
		return false;
	}

	UPROPERTY(Transient)
	TMap<FGameplayTag, FPassiveListenerGrpList> m_tmapPassiveListeners;

	UPROPERTY(Transient)
	TSet<FGameplayTag> m_tsetUnlockTags;
};
