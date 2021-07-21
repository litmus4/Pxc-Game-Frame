// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/002_GroupCentral/TestGroupCentralComponent.h"

UTestGroupCentralComponent::UTestGroupCentralComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	m_pDefaultDynamic = nullptr;
	m_pOwnerDynamic = nullptr;

	//
}

void UTestGroupCentralComponent::BeginPlay()
{
	Super::BeginPlay();

	//
}

bool UTestGroupCentralComponent::CastParameter(const FSharedSignature& ParamSig, FTestGroupCentralParameter& OutParam)
{
	if (ParamSig.IsDerived<FTestGroupCentralParameter>())
	{
		OutParam = *ParamSig.GetDerivedPtr<FTestGroupCentralParameter>();
		return true;
	}
	return false;
}