// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/VirtualGroupMgr.h"

void FVirtualGroup::AddActor(AActor* pActor)
{
	check(pActor);
	tarrActors.AddUnique(pActor);
}

void FVirtualGroup::AddActors(const TArray<AActor*>& xActors)
{
	for (AActor* pCurActor : xActors)
	{
		check(pCurActor);
		tarrActors.AddUnique(pCurActor);
	}
}

void FVirtualGroup::RemoveActor(AActor* pActor)
{
	check(pActor);
	tarrActors.Remove(pActor);
}

void FVirtualGroup::ClearActors()
{
	tarrActors.Empty();
}

void FVirtualGroup::AddFeature(FVirtGrpFeature* pFeature)
{
	check(pFeature);
	mapFeatures.insert(std::make_pair(pFeature->GetUsage(), pFeature));
}

void FVirtualGroup::AddFeatures(const std::vector<FVirtGrpFeature*>& vecFeatures)
{
	std::vector<FVirtGrpFeature*>::const_iterator iter = vecFeatures.begin();
	for (; iter != vecFeatures.end(); iter++)
	{
		check(*iter);
		mapFeatures.insert(std::make_pair((*iter)->GetUsage(), *iter));
	}
}

void FVirtualGroup::RemoveFeatureByUsage(EVirtualGroupUsage eUsage)
{
	mapFeatures.erase(eUsage);
}

void FVirtualGroup::ClearFeatures()
{
	mapFeatures.clear();
}