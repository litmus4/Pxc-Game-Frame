// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "../Structs/VirtualGroupStructs.h"
#include <unordered_map>
#include "VirtualGroupMgr.generated.h"

USTRUCT()
struct FVirtualGroup
{
	GENERATED_BODY()
public:
	FVirtualGroup() = default;
	FVirtualGroup(const FName& xName) : Name(xName) {}

	void AddActor(AActor* pActor);
	void AddActors(const TArray<AActor*>& xActors);
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
	TArray<AActor*> tarrActors;

	std::unordered_map<EVirtualGroupUsage, FVirtGrpFeature*> mapFeatures;
};

/**
 * 
 */
UCLASS()
class UE4GAME_API UVirtualGroupMgr : public UObject
{
	GENERATED_BODY()
	
public:

private:
	//
};
