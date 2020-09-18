// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayTagResponseTable.h"

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
	m_pNowAppliedGE = pSpec->Def;
}