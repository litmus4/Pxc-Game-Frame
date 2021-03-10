// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Structs/VirtualGroupStructs.h"
#include <map>
#include <unordered_map>
#include <set>
#include <functional>
#include "VirtualGroupMgr.generated.h"

USTRUCT()
struct FVirtualGroup
{
	GENERATED_BODY()
public:
	FVirtualGroup() = default;
	FVirtualGroup(const FName& xName) : Name(xName) {}

	void AddActor(AActor* pActor);
	void AddActors(const TArray<AActor*>& tarrActors);
	bool HasActor(AActor* pActor);
	void RemoveActor(AActor* pActor);
	void ClearActors();

	void AddFeature(FVirtGrpFeature* pFeature);
	void AddFeatures(const std::vector<FVirtGrpFeature*>& vecFeatures);
	void RemoveFeatureByUsage(EVirtualGroupUsage eUsage);
	void ClearFeatures();

	template<class T>
	T* GetFeatureByUsage(EVirtualGroupUsage eUsage)
	{
		static_assert(TPointerIsConvertibleFromTo<T, FVirtGrpFeature>::Value,
			"FVirtualGroup GetFeatureByUsage static_assert");
		std::unordered_map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator iter = mapFeatures.find(eUsage);
		if (iter != mapFeatures.end())
			return static_cast<T*>(iter->second);
		return nullptr;
	}

	UPROPERTY()
	FName Name;

	UPROPERTY()
	TSet<AActor*> tsetActors;

	std::map<EVirtualGroupUsage, FVirtGrpFeature*> mapFeatures;
};

/**
 * 
 */
UCLASS()
class UE4GAME_API UVirtualGroupMgr : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Gameplay, meta = (DisplayName = "CreateGroup", ScriptName = "CreateGroup"))
	bool K2_CreateGroup(FName Name, const TArray<EVirtualGroupUsage>& tarrInitialUsages);

	bool CreateGroup(const FName& Name, const TArray<EVirtualGroupUsage>& tarrInitialUsages, FVirtualGroup** ppOutGroup = nullptr);
	FVirtualGroup* GetGroup(const FName& Name);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void RemoveGroup(const FName& Name);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void Clear();

	template<class T>
	void ForEachGroupOfUsage(EVirtualGroupUsage eUsage, std::function<void(FVirtualGroup*, T*)>& fnOnEach)
	{
		std::unordered_map<EVirtualGroupUsage, std::vector<FName>>::iterator itU2g = m_mapUsageToGroups.find(eUsage);
		if (itU2g == m_mapUsageToGroups.end())
			return;

		std::vector<FName>::iterator itName = itU2g->second.begin();
		for (; itName != itU2g->second.end(); itName++)
		{
			FVirtualGroup* pGroup = m_tmapGroups.Find(*itName);
			if (!pGroup) continue;
			T* pFeature = pGroup->GetFeatureByUsage<T>(eUsage);
			fnOnEach(pGroup, pFeature);
		}
	}

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void AddActorToGroup(AActor* pActor, const FName& GroupName);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void AddActorsToGroup(const TArray<AActor*>& tarrActors, const FName& GroupName);

	UFUNCTION(BlueprintPure, Category = Gameplay)
	bool HasActorInGroup(AActor* pActor, const FName& GroupName);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void RemoveActorFromGroup(AActor* pActor, const FName& GroupName);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ClearActorsOfGroup(const FName& GroupName);

	void Release();

private:
	FVirtGrpFeature* NewFeature(EVirtualGroupUsage eUsage);

	UPROPERTY()
	TMap<FName, FVirtualGroup> m_tmapGroups;

	std::unordered_map<EVirtualGroupUsage, std::set<FName>> m_mapUsageToGroups;
	std::unordered_map<AActor*, std::set<FName>> m_mapActorToGroups;
};