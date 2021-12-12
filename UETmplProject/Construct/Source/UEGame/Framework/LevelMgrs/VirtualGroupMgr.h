// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Structs/VirtualGroupStructs.h"
#include "../Structs/ScatteredStructs.h"
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
		std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator iter = mapFeatures.find(eUsage);
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
class UEGAME_API UVirtualGroupMgr : public UObject
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
	bool ForEachGroupOfUsage(EVirtualGroupUsage eUsage, std::function<bool(FVirtualGroup*, T*)> fnOnEach)
	{
		std::unordered_map<EVirtualGroupUsage, std::set<FName4Stl>>::iterator itU2g = m_mapUsageToGroups.find(eUsage);
		if (itU2g == m_mapUsageToGroups.end())
			return false;

		std::set<FName4Stl>::iterator itName = itU2g->second.begin();
		for (; itName != itU2g->second.end(); itName++)
		{
			FVirtualGroup* pGroup = m_tmapGroups.Find(itName->N);
			if (!pGroup) continue;
			T* pFeature = pGroup->GetFeatureByUsage<T>(eUsage);
			if (fnOnEach(pGroup, pFeature)) return true;
		}
		return false;
	}

	template<class T>
	bool ForEachGroupWithActor(AActor* pActor, EVirtualGroupUsage eSelectUsage, std::function<bool(FVirtualGroup*, T*)> fnOnEach)
	{
		std::unordered_map<AActor*, std::set<FName4Stl>>::iterator itA2g = m_mapActorToGroups.find(pActor);
		if (!pActor || itA2g == m_mapActorToGroups.end())
			return false;

		std::set<FName4Stl>::iterator itName = itA2g->second.begin();
		for (; itName != itA2g->second.end(); itName++)
		{
			FVirtualGroup* pGroup = m_tmapGroups.Find(itName->N);
			if (!pGroup) continue;

			if (eSelectUsage != EVirtualGroupUsage::Unknown)
			{
				T* pFeature = pGroup->GetFeatureByUsage<T>(eSelectUsage);
				if (pFeature)
					if (fnOnEach(pGroup, pFeature)) return true;
			}
			else
				if (fnOnEach(pGroup, nullptr)) return true;
		}
		return false;
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

	FVirtGrpFeature* AddFeatureToGroup(EVirtualGroupUsage eUsage, const FName& GroupName);

	UFUNCTION(BlueprintCallable, Category = Gameplay, meta = (DisplayName = "AddFeatureToGroup", ScriptName = "AddFeatureToGroup"))
	void K2_AddFeatureToGroup(EVirtualGroupUsage eUsage, const FName& GroupName);

	template<class T>
	T* GetFeatureFromGroup(EVirtualGroupUsage eUsage, const FName& GroupName)
	{
		FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
		if (pGroup)
			return pGroup->GetFeatureByUsage<T>(eUsage);
		return nullptr;
	}

	UFUNCTION(BlueprintPure, Category = Gameplay)
	bool GetRTDFeatureFromGroup(const FName& GroupName, FVirtGrpRTDFeature& OutFeature);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void RemoveFeatureFromGroup(EVirtualGroupUsage eUsage, const FName& GroupName);

	void Release();

private:
	FVirtGrpFeature* NewFeature(EVirtualGroupUsage eUsage);

	UPROPERTY()
	TMap<FName, FVirtualGroup> m_tmapGroups;

	std::unordered_map<EVirtualGroupUsage, std::set<FName4Stl>> m_mapUsageToGroups;
	std::unordered_map<AActor*, std::set<FName4Stl>> m_mapActorToGroups;
};