// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "PxcAssistantSubsystem.generated.h"

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
class UE4GAME_API UPxcAssistantSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
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

	}

private:
	UPROPERTY(Transient)
	TMap<FActorWithKey, UObject*> m_tmapWaitingActorKeys;
};
