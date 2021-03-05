// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/VirtualGroupMgr.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/Scattered.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

void FVirtualGroup::AddActor(AActor* pActor)
{
	check(pActor);
	tsetActors.Add(pActor);
}

void FVirtualGroup::AddActors(const TArray<AActor*>& xActors)
{
	for (AActor* pActor : xActors)
	{
		check(pActor);
		tsetActors.Add(pActor);
	}
}

bool FVirtualGroup::HasActor(AActor* pActor)
{
	check(pActor);
	return tsetActors.Contains(pActor);
}

void FVirtualGroup::RemoveActor(AActor* pActor)
{
	check(pActor);
	tsetActors.Remove(pActor);
}

void FVirtualGroup::ClearActors()
{
	tsetActors.Empty();
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
	std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator iter = mapFeatures.find(eUsage);
	if (iter != mapFeatures.end())
	{
		SAFE_DELETE(iter->second)
		mapFeatures.erase(iter);
	}
}

void FVirtualGroup::ClearFeatures()
{
	std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator iter = mapFeatures.begin();
	for (; iter != mapFeatures.end(); iter++)
		SAFE_DELETE(iter->second)
	mapFeatures.clear();
}

bool UVirtualGroupMgr::K2_CreateGroup(const FName& Name, const TArray<EVirtualGroupUsage>& tarrInitialUsages)
{
	return CreateGroup(Name, tarrInitialUsages);
}

bool UVirtualGroupMgr::CreateGroup(const FName& Name, const TArray<EVirtualGroupUsage>& tarrInitialUsages, FVirtualGroup** ppOutGroup)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(Name);
	if (pGroup)
	{
		if (ppOutGroup) *ppOutGroup = pGroup;
		return false;
	}

	pGroup = &m_tmapGroups.Add(Name);
	pGroup->Name = Name;
	for (EVirtualGroupUsage eUsage : tarrInitialUsages)
	{
		FVirtGrpFeature* pFeature = NewFeature(eUsage);
		if (ensureMsgf(pFeature, TEXT("UVirtualGroupMgr CreateGroup: Invalid initial usage!")))
		{
			pGroup->AddFeature(pFeature);

			std::unordered_map<EVirtualGroupUsage, std::vector<FName>>::iterator iter = m_mapUsageToGroups.find(eUsage);
			if (iter == m_mapUsageToGroups.end())
				iter = m_mapUsageToGroups.insert(std::make_pair(eUsage, std::vector<FName>())).first;
			iter->second.push_back(Name);
		}
	}
	if (ppOutGroup) *ppOutGroup = pGroup;
	return true;
}

FVirtualGroup* UVirtualGroupMgr::GetGroup(const FName& Name)
{
	return m_tmapGroups.Find(Name);
}

void UVirtualGroupMgr::RemoveGroup(const FName& Name)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(Name);
	if (!pGroup) return;

	std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator itFeature = pGroup->mapFeatures.begin();
	for (; itFeature != pGroup->mapFeatures.end(); itFeature++)
	{
		std::unordered_map<EVirtualGroupUsage, std::vector<FName>>::iterator itU2g = m_mapUsageToGroups.find(itFeature->first);
		if (itU2g == m_mapUsageToGroups.end())
			continue;
		std::vector<FName>::iterator itName = std::find(itU2g->second.begin(), itU2g->second.end(), Name);
		if (itName != itU2g->second.end())
		{
			itU2g->second.erase(itName);
			if (itU2g->second.empty())
				m_mapUsageToGroups.erase(itU2g);
		}
	}
	pGroup->ClearFeatures();
	m_tmapGroups.Remove(Name);
}

void UVirtualGroupMgr::Clear()
{
	for (auto& Pair : m_tmapGroups)
		Pair.Value.ClearFeatures();
	m_tmapGroups.Empty();
	m_mapUsageToGroups.clear();
}

void UVirtualGroupMgr::Release()
{
	Clear();
}

FVirtGrpFeature* UVirtualGroupMgr::NewFeature(EVirtualGroupUsage eUsage)
{
	switch (eUsage)
	{
	case EVirtualGroupUsage::RelativeTimeDilation:
		return new FVirtGrpRTDFeature();
	}
	return nullptr;
}