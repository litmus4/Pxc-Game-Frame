// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Roles/PxcLogicRole.h"
#include "Framework/PxcGameConfig.h"
#include "Framework/PxcStaticResourceSystem.h"
#include "Framework/PxcBlueprintLibrary.h"
#include "GameplayAbility/PxcAbilitySystemComponent.h"
#include "GameplayAbility/PxcAbilitySystemGlobals.h"
#include "GameplayAbility/Effects/ContextGameplayEffect.h"
#include "GameplayAbility/ExecutionCalculations/ExtentionExecution.h"

// Sets default values
APxcLogicRole::APxcLogicRole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pPASC = CreateDefaultSubobject<UPxcAbilitySystemComponent>(TEXT("PASComp"));
}

UPxcAbilitySystemComponent* APxcLogicRole::GetAbilitySystemComponent()
{
	return m_pPASC;
}

// Called when the game starts or when spawned
void APxcLogicRole::BeginPlay()
{
	Super::BeginPlay();
	
	UPxcAbilitySystemComponent* pPASC = GetAbilitySystemComponent();
	check(pPASC);
	pPASC->OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &APxcLogicRole::OnGameplayEffectApplied);
	pPASC->OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &APxcLogicRole::OnGameplayEffectRemoved);
}

void APxcLogicRole::OnGameplayEffectApplied(UAbilitySystemComponent* pASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle)
{
	if (!Spec.Def) return;

	const UContextGameplayEffect* pCGE = Cast<UContextGameplayEffect>(Spec.Def);
	if (pCGE)
		const_cast<UContextGameplayEffect*>(pCGE)->AddContextAndHandle(pASC, Spec.GetContext(), Handle);

	UPxcAbilitySystemGlobals* pPASG = Cast<UPxcAbilitySystemGlobals>(&UAbilitySystemGlobals::Get());
	if (pPASG)
		pPASG->RemoveGEContModify(pASC, Spec.Def);

	const FGameplayTagContainer& UnlockParentTags = GetDefault<UPxcGameConfig>()->UnlockGameplayEffectParentTags;
	if (Spec.Def->InheritableGameplayEffectTags.CombinedTags.HasAny(UnlockParentTags))//使用的是Parent判断
	{
		UPxcAbilitySystemComponent* pPASC = Cast<UPxcAbilitySystemComponent>(pASC);
		if (pPASC) pPASC->UpdateUnlockTags();
	}

	static const FGameplayTag ExTag = FGameplayTag::RequestGameplayTag(TEXT("Buff.Extention"));
	TArray<FGameplayTag> tarrGETags;
	Spec.Def->InheritableOwnedTagsContainer.CombinedTags.GetGameplayTagArray(tarrGETags);
	for (FGameplayTag& Tag : tarrGETags)
	{
		if (Tag.MatchesTag(ExTag) && Tag != ExTag)
		{
			UPxcStaticResourceSystem* pSRS = UPxcStaticResourceSystem::GetInstance();
			const FUTRGExContToInst* pRow = pSRS->GetUTRGEExtentionContToInst(Tag);
			if (pRow && pASC)
			{
				auto SpecHandle = pASC->MakeOutgoingSpec(pRow->InstantEffect, 1, pASC->MakeEffectContext());
				TMap<FString, float> tmapRet;
				auto RetHandle = UPxcBlueprintLibrary::ApplyExtentionGESpecWithReturn(pASC, SpecHandle, Handle, tmapRet);
				if (RetHandle.WasSuccessfullyApplied())
					const_cast<FGameplayEffectSpec&>(Spec).CopySetByCallerMagnitudes(*SpecHandle.Data);
			}
			break;
		}
	}
}

void APxcLogicRole::OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveEffect)
{
	UPxcAbilitySystemComponent* pPASC = GetAbilitySystemComponent();
	//check(pPASC);
	if (!IsValid(pPASC)) return;

	const FGameplayTagContainer& UnlockParentTags = GetDefault<UPxcGameConfig>()->UnlockGameplayEffectParentTags;
	if (ActiveEffect.Spec.Def->InheritableGameplayEffectTags.CombinedTags.HasAny(UnlockParentTags))//使用的是Parent判断
		pPASC->UpdateUnlockTags();

	const static FGameplayTag ExTag = FGameplayTag::RequestGameplayTag(TEXT("Buff.Extention"));
	TArray<FGameplayTag> tarrGETags;
	ActiveEffect.Spec.Def->InheritableOwnedTagsContainer.CombinedTags.GetGameplayTagArray(tarrGETags);
	for (FGameplayTag& Tag : tarrGETags)
	{
		if (Tag.MatchesTag(ExTag) && Tag != ExTag)
		{
			UExtentionExecution::Discard(pPASC, ActiveEffect.Spec, ActiveEffect.Handle);
			break;
		}
	}

	const UContextGameplayEffect* pCGE = Cast<UContextGameplayEffect>(ActiveEffect.Spec.Def);
	if (pCGE)
	{
		int32 iStackCount = pPASC->GetCurrentStackCount(ActiveEffect.Handle);
		if (iStackCount <= 1)
			const_cast<UContextGameplayEffect*>(pCGE)->RemoveContextPair(pPASC);
	}
}

// Called every frame
void APxcLogicRole::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

}

