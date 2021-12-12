// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcEngineSubsystem.h"
#include "Engine/Engine.h"
#include "AbilitySystemGlobals.h"
#include "Misc/CoreDelegates.h"

void UPxcEngineSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (GEngine && GEngine->IsInitialized())
	{
		UAbilitySystemGlobals* pASG = &UAbilitySystemGlobals::Get();
		pASG->InitGlobalData();
	}
	else
		DelePostEngineInitHandle = FCoreDelegates::OnPostEngineInit.AddUObject(this, &ThisClass::OnPostEngineInit);
}

void UPxcEngineSubsystem::Deinitialize()
{
	if (DelePostEngineInitHandle.IsValid())
	{
		FCoreDelegates::OnPostEngineInit.Remove(DelePostEngineInitHandle);
		DelePostEngineInitHandle.Reset();
	}
}

void UPxcEngineSubsystem::OnPostEngineInit()
{
	UAbilitySystemGlobals* pASG = &UAbilitySystemGlobals::Get();
	pASG->InitGlobalData();
}