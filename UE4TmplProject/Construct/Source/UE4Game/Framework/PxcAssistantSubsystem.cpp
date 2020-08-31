// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAssistantSubsystem.h"
#include "Utilities/AsyncWaitActorWithKey.h"

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

UPxcAssistantSubsystem* UPxcAssistantSubsystem::s_pInst = nullptr;

UPxcAssistantSubsystem* UPxcAssistantSubsystem::GetInstance()
{
	check(s_pInst);
	return s_pInst;
}

void UPxcAssistantSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	s_pInst = this;
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

void UPxcAssistantSubsystem::FinishAsyncActorWithKey(AActor* pActor, const FString& sKey)
{
	EndWaitingActorWithKey<UAsyncWaitActorWithKey>(pActor, sKey, true);
}