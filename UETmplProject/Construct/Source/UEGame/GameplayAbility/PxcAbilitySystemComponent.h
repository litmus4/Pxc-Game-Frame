// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "PrivateDefinitions/GameplayAbilityDef.h"
#include "Abilities/PassiveListenerStructs.h"
#include <vector>
#include <map>
#include <string>
#include <functional>
#include "PxcAbilitySystemComponent.generated.h"

USTRUCT()
struct FPassiveListenerGrpList
{
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<FPassiveListenerGroup> tarr;
};

struct SGEExtentionStack
{
	SGEExtentionStack() : fUnique(0.0f) {}

	std::vector<float> vecStack;
	float fUnique;
};

struct SGEExtentionStackMap
{
	SGEExtentionStackMap() : iStackedNum(0) {}

	std::map<std::string, SGEExtentionStack> map;
	int32 iStackedNum;
};

/**
 * 
 */
UCLASS(Blueprintable)
class UEGAME_API UPxcAbilitySystemComponent : public UAbilitySystemComponent
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

	UFUNCTION(BlueprintNativeEvent)
	void ExecuteGEExtentionByType(EGEExtentionType eType, float fVariable, TMap<FString, float>& tmapOutNames);
	virtual void ExecuteGEExtentionByType_Implementation(EGEExtentionType eType, float fVariable, TMap<FString, float>& tmapOutNames);

	UFUNCTION(BlueprintNativeEvent)
	void DiscardGEExtentionByType(EGEExtentionType eType, const TMap<FString, float>& tmapFakeSpec);
	virtual void DiscardGEExtentionByType_Implementation(EGEExtentionType eType, const TMap<FString, float>& tmapFakeSpec);

	void ForEachGEExtentionReturnToStack(const FActiveGameplayEffectHandle& Handle, std::function<void(const std::string&, float)>&& fnOnEach);
	void PopGEExtentionStackByHandle(const FActiveGameplayEffectHandle& Handle, TMap<FString, float>& tmapPopped);

	void UpdateUnlockTags();

	UFUNCTION(BlueprintPure, Category = Gameplay)
	bool IsUnlocked(FGameplayTag Tag);

	UPROPERTY(BlueprintAssignable)
	FAbilityGivenDelegate DeleAbilityGiven;

	UPROPERTY(BlueprintAssignable)
	FAbilityGivenDelegate DeleAbilityRemoved;

	std::map<std::string, float> m_mapGEExtentionReturn;

protected:
	virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	virtual void OnRemoveAbility(FGameplayAbilitySpec& AbilitySpec) override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnGiveAbility(const FGameplayAbilitySpec& AbilitySpec);

	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnRemoveAbility(const FGameplayAbilitySpec& AbilitySpec);

	template<class T>
	bool ProcessOnePassiveGroup(FPassiveListenerGroup& Group, int64 lUid, bool bRestart, std::function<void(T*)>&& fnProc)
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

	UFUNCTION(BlueprintNativeEvent)
	bool IsGEExtentionStack(const FString& sName) const;
	virtual bool IsGEExtentionStack_Implementation(const FString& sName) const;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FPassiveListenerGrpList> m_tmapPassiveListeners;

	UPROPERTY(Transient)
	TSet<FGameplayTag> m_tsetUnlockTags;

	TMap<FActiveGameplayEffectHandle, SGEExtentionStackMap> m_tmapGEExtentionStackTotal;
};