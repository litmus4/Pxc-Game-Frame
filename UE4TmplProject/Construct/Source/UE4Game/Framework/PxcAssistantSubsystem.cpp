// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAssistantSubsystem.h"

bool FActorWithKey::operator==(const FActorWithKey& Other) const
{
	return pActor == Other.pActor && sKey == Other.sKey;
}

bool FActorWithKey::operator!=(const FActorWithKey& Other) const
{
	return !(*this == Other);
}

uint32 GetTypeHash(const FActorWithKey& In)
{
	uint32 uHash1 = GetTypeHash(In.pActor);
	uint32 uHash2 = GetTypeHash(In.sKey);
	return HashCombine(uHash1, uHash2);
}

void UPxcAssistantSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	//
}

void UPxcAssistantSubsystem::Deinitialize()
{
	//
}

bool UPxcAssistantSubsystem::AddWaitingActorWithKey(AActor* pActor, const FString& sKey, UObject* pObject)
{
	FActorWithKey ActorKey(pActor, sKey);
	if (!m_tmapWaitingActorKeys.Contains(ActorKey))
	{
		m_tmapWaitingActorKeys.Add(ActorKey, pObject);
		return true;
	}
	return false;
}