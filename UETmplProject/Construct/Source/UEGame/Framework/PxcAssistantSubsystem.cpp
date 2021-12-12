// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcAssistantSubsystem.h"
#include "Utilities/AsyncWaitActorWithKey.h"
#include "PxcUtil/IDPool.h"
#include "PxcUtil/IDPool64.h"

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
	uint32&& uHash1 = GetTypeHash(In.pActor);
	uint32&& uHash2 = GetTypeHash(In.sKey);
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
	std::unordered_map<EUidPoolType, PxcUtil::CIDPool*>::iterator it32 = m_mapUidPools.begin();
	for (; it32 != m_mapUidPools.end(); it32++)
		delete it32->second;
	m_mapUidPools.clear();

	std::unordered_map<EUidPoolType, PxcUtil::CIDPool64*>::iterator it64 = m_mapUidPool64s.begin();
	for (; it64 != m_mapUidPool64s.end(); it64++)
		delete it64->second;
	m_mapUidPool64s.clear();

	std::unordered_map<std::string, PxcUtil::CIDPool*>::iterator itis = m_mapIdStackUidPools.begin();
	for (; itis != m_mapIdStackUidPools.end(); itis++)
		delete itis->second;
	m_mapIdStackUidPools.clear();
}

bool UPxcAssistantSubsystem::AddWaitingActorWithKey(AActor* pActor, const FString& sKey, UObject* pObject)
{
	FActorWithKey ActorKey(pActor, sKey);
	if (!m_tmapWaitingActorKeys.Contains(ActorKey))
	{
		m_tmapWaitingActorKeys.Add(ActorKey, pObject);
		return true;
	}
	m_tmapWaitingActorKeys.Remove(ActorKey);
	return false;
}

void UPxcAssistantSubsystem::FinishAsyncActorWithKey(AActor* pActor, const FString& sKey)
{
	EndWaitingActorWithKey<UAsyncWaitActorWithKey>(pActor, sKey, true);
}

PxcUtil::CIDPool* UPxcAssistantSubsystem::GetUidPool(EUidPoolType eType)
{
	std::unordered_map<EUidPoolType, PxcUtil::CIDPool*>::iterator iter = m_mapUidPools.find(eType);
	if (iter != m_mapUidPools.end())
		return iter->second;

	if (!IsUidPool64(eType))
	{
		PxcUtil::CIDPool* pIDPool = new PxcUtil::CIDPool(-1, INT32_MAX, -1);
		m_mapUidPools.insert(std::make_pair(eType, pIDPool));
		return pIDPool;
	}
	return nullptr;
}

PxcUtil::CIDPool64* UPxcAssistantSubsystem::GetUidPool64(EUidPoolType eType)
{
	std::unordered_map<EUidPoolType, PxcUtil::CIDPool64*>::iterator iter = m_mapUidPool64s.find(eType);
	if (iter != m_mapUidPool64s.end())
		return iter->second;

	if (IsUidPool64(eType))
	{
		PxcUtil::CIDPool64* pIDPool = new PxcUtil::CIDPool64(-1LL, INT64_MAX, -1LL);
		m_mapUidPool64s.insert(std::make_pair(eType, pIDPool));
		return pIDPool;
	}
	return nullptr;
}

PxcUtil::CIDPool* UPxcAssistantSubsystem::GetIdentifiedStackUidPool(const std::string& strKey)
{
	std::unordered_map<std::string, PxcUtil::CIDPool*>::iterator iter = m_mapIdStackUidPools.find(strKey);
	if (iter != m_mapIdStackUidPools.end())
		return iter->second;

	PxcUtil::CIDPool* pIDPool = new PxcUtil::CIDPool(-1, INT32_MAX, -1);
	m_mapIdStackUidPools.insert(std::make_pair(strKey, pIDPool));
	return pIDPool;
}

bool UPxcAssistantSubsystem::IsUidPool64(EUidPoolType eType)
{
	switch (eType)
	{
	case EUidPoolType::PointerModelLL:
		return true;
	}
	return false;
}