// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include <unordered_map>
#include "PxcAssistantSubsystem.generated.h"

namespace PxcUtil
{
	class CIDPool;
	class CIDPool64;
}

USTRUCT()
struct FActorWithKey
{
	GENERATED_BODY()
public:
	FActorWithKey() : pActor(nullptr) {}
	FActorWithKey(AActor* xActor, const FString& xKey)
		: pActor(xActor), sKey(xKey)
	{}

	bool operator==(const FActorWithKey& Other) const;
	bool operator!=(const FActorWithKey& Other) const;
	inline friend uint32 GetTypeHash(const FActorWithKey& In);

	UPROPERTY()
	AActor* pActor;

	UPROPERTY()
	FString sKey;
};

/**
 * 
 */
UCLASS()
class UEGAME_API UPxcAssistantSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	enum EUidPoolType
	{
		PointerModelLL,
		RelativeTimeDilation,
	};
#define ASUIDPOOL UPxcAssistantSubsystem::EUidPoolType
	
public:
	static UPxcAssistantSubsystem* GetInstance();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	bool AddWaitingActorWithKey(AActor* pActor, const FString& sKey, UObject* pObject);

	template<class T>
	void EndWaitingActorWithKey(AActor* pActor, const FString& sKey, bool bFinish)
	{
		FActorWithKey ActorKey(pActor, sKey);
		auto ppObject = m_tmapWaitingActorKeys.Find(ActorKey);
		T* pObject = Cast<T>(ppObject ? *ppObject : nullptr);
		if (IsValid(pObject))
		{
			pObject->OnEnded(bFinish);
			m_tmapWaitingActorKeys.Remove(ActorKey);
		}
		else
			m_tmapWaitingActorKeys.Add(ActorKey, nullptr);

	}

	template<class T>
	T* GetObjectOfActorWithKey(AActor* pActor, const FString& sKey)
	{
		FActorWithKey ActorKey;
		ActorKey.pActor = pActor;
		ActorKey.sKey = sKey;
		UObject** ppObject = m_tmapWaitingActorKeys.Find(ActorKey);
		return Cast<T>(ppObject ? *ppObject : nullptr);
	}

	UFUNCTION(BlueprintCallable)
	void FinishAsyncActorWithKey(AActor* pActor, const FString& sKey);

	PxcUtil::CIDPool* GetUidPool(EUidPoolType eType);
	PxcUtil::CIDPool64* GetUidPool64(EUidPoolType eType);
	PxcUtil::CIDPool* GetIdentifiedStackUidPool(const std::string& strKey);

private:
	bool IsUidPool64(EUidPoolType eType);

	static UPxcAssistantSubsystem* s_pInst;

	UPROPERTY(Transient)
	TMap<FActorWithKey, UObject*> m_tmapWaitingActorKeys;

	std::unordered_map<EUidPoolType, PxcUtil::CIDPool*> m_mapUidPools;
	std::unordered_map<EUidPoolType, PxcUtil::CIDPool64*> m_mapUidPool64s;
	std::unordered_map<std::string, PxcUtil::CIDPool*> m_mapIdStackUidPools;
};
