// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Systems/RandomGameplaySystem.h"
#include "GameplayRelated/Random/RandomModeComponent.h"

void URandomGameplaySystem::GenerateZGNLayers(TSubclassOf<URandomModeComponent> cComponent)
{
	ClearZGNLayers();

	URandomModeComponent* pDefComp = cComponent.GetDefaultObject();
	if (!IsValid(pDefComp)) return;
	VerifyZGNComponent(pDefComp);

	//
}

void URandomGameplaySystem::GetZGNLayers(TArray<FRandomZGNLayer>& tarrOutZGNLayers)
{
	tarrOutZGNLayers = m_tarrZGNLayers;
}

void URandomGameplaySystem::ClearZGNLayers()
{
	m_tarrZGNLayers.Empty();
}

void URandomGameplaySystem::Release()
{
	ClearZGNLayers();
}

void URandomGameplaySystem::VerifyZGNComponent(URandomModeComponent* pComp)
{
	//
}