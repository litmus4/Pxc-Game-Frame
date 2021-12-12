// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/VirtualGroupMgr.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/Scattered.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

void FVirtualGroup::AddActor(AActor* pActor)
{
	verify(IsValid(pActor));
	tsetActors.Add(pActor);
}

void FVirtualGroup::AddActors(const TArray<AActor*>& tarrActors)
{
	for (AActor* pActor : tarrActors)
	{
		verify(IsValid(pActor));
		tsetActors.Add(pActor);
	}
}

bool FVirtualGroup::HasActor(AActor* pActor)
{
	verify(IsValid(pActor));
	return tsetActors.Contains(pActor);
}

void FVirtualGroup::RemoveActor(AActor* pActor)
{
	verify(IsValid(pActor));
	tsetActors.Remove(pActor);
}

void FVirtualGroup::ClearActors()
{
	tsetActors.Empty();
}

void FVirtualGroup::AddFeature(FVirtGrpFeature* pFeature)
{
	verify(pFeature);
	mapFeatures.insert(std::make_pair(pFeature->GetUsage(), pFeature));
}

void FVirtualGroup::AddFeatures(const std::vector<FVirtGrpFeature*>& vecFeatures)
{
	std::vector<FVirtGrpFeature*>::const_iterator iter = vecFeatures.begin();
	for (; iter != vecFeatures.end(); iter++)
	{
		verify(*iter);
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

bool UVirtualGroupMgr::K2_CreateGroup(FName Name, const TArray<EVirtualGroupUsage>& tarrInitialUsages)
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
	FName4Stl Name4(Name);
	for (EVirtualGroupUsage eUsage : tarrInitialUsages)
	{
		FVirtGrpFeature* pFeature = NewFeature(eUsage);
		if (ensureMsgf(pFeature, TEXT("UVirtualGroupMgr CreateGroup: Invalid initial usage!")))
		{
			pFeature->GroupName = Name;
			pGroup->AddFeature(pFeature);

			std::unordered_map<EVirtualGroupUsage, std::set<FName4Stl>>::iterator iter = m_mapUsageToGroups.find(eUsage);
			if (iter == m_mapUsageToGroups.end())
				iter = m_mapUsageToGroups.insert(std::make_pair(eUsage, std::set<FName4Stl>())).first;

			ensureMsgf(iter->second.insert(Name4).second,
				TEXT("UVirtualGroupMgr CreateGroup: Repeatedly add feature to same group!"));
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
	FName4Stl Name4(Name);

	for (AActor* pActor : pGroup->tsetActors)
	{
		std::unordered_map<AActor*, std::set<FName4Stl>>::iterator itA2g = m_mapActorToGroups.find(pActor);
		if (itA2g == m_mapActorToGroups.end())
			continue;
		std::set<FName4Stl>::iterator itName = itA2g->second.find(Name4);
		if (itName != itA2g->second.end())
		{
			itA2g->second.erase(itName);
			if (itA2g->second.empty())
				m_mapActorToGroups.erase(itA2g);
		}
	}
	pGroup->ClearActors();

	std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator itFeature = pGroup->mapFeatures.begin();
	for (; itFeature != pGroup->mapFeatures.end(); itFeature++)
	{
		std::unordered_map<EVirtualGroupUsage, std::set<FName4Stl>>::iterator itU2g = m_mapUsageToGroups.find(itFeature->first);
		if (itU2g == m_mapUsageToGroups.end())
			continue;
		std::set<FName4Stl>::iterator itName = itU2g->second.find(Name4);
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
	m_mapActorToGroups.clear();
}

void UVirtualGroupMgr::AddActorToGroup(AActor* pActor, const FName& GroupName)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
	if (pGroup)
	{
		pGroup->AddActor(pActor);

		std::unordered_map<AActor*, std::set<FName4Stl>>::iterator iter = m_mapActorToGroups.find(pActor);
		if (iter == m_mapActorToGroups.end())
			iter = m_mapActorToGroups.insert(std::make_pair(pActor, std::set<FName4Stl>())).first;

		ensureMsgf(iter->second.insert(FName4Stl(GroupName)).second,
			TEXT("UVirtualGroupMgr AddActorToGroup: Repeatedly add actor to same group!"));

		UWorld* pWorld = (GetOuter() ? GetOuter()->GetWorld() : nullptr);
		std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator itFeature = pGroup->mapFeatures.begin();
		for (; itFeature != pGroup->mapFeatures.end(); itFeature++)
			itFeature->second->OnActorUpdated(pWorld, pGroup->tsetActors);
	}
}

void UVirtualGroupMgr::AddActorsToGroup(const TArray<AActor*>& tarrActors, const FName& GroupName)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
	if (pGroup)
	{
		pGroup->AddActors(tarrActors);

		FName4Stl Name4(GroupName);
		for (AActor* pActor : tarrActors)
		{
			std::unordered_map<AActor*, std::set<FName4Stl>>::iterator iter = m_mapActorToGroups.find(pActor);
			if (iter == m_mapActorToGroups.end())
				iter = m_mapActorToGroups.insert(std::make_pair(pActor, std::set<FName4Stl>())).first;

			ensureMsgf(iter->second.insert(Name4).second,
				TEXT("UVirtualGroupMgr AddActorsToGroup: Repeatedly add actor to same group!"));
		}

		UWorld* pWorld = (GetOuter() ? GetOuter()->GetWorld() : nullptr);
		std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator itFeature = pGroup->mapFeatures.begin();
		for (; itFeature != pGroup->mapFeatures.end(); itFeature++)
			itFeature->second->OnActorUpdated(pWorld, pGroup->tsetActors);
	}
}

bool UVirtualGroupMgr::HasActorInGroup(AActor* pActor, const FName& GroupName)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
	if (pGroup)
		return pGroup->HasActor(pActor);
	return false;
}

void UVirtualGroupMgr::RemoveActorFromGroup(AActor* pActor, const FName& GroupName)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
	if (pGroup)
	{
		pGroup->RemoveActor(pActor);

		std::unordered_map<AActor*, std::set<FName4Stl>>::iterator itA2g = m_mapActorToGroups.find(pActor);
		if (itA2g != m_mapActorToGroups.end())
		{
			std::set<FName4Stl>::iterator itName = itA2g->second.find(FName4Stl(GroupName));
			if (itName != itA2g->second.end())
			{
				itA2g->second.erase(itName);
				if (itA2g->second.empty())
					m_mapActorToGroups.erase(itA2g);
			}
		}

		UWorld* pWorld = (GetOuter() ? GetOuter()->GetWorld() : nullptr);
		std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator itFeature = pGroup->mapFeatures.begin();
		for (; itFeature != pGroup->mapFeatures.end(); itFeature++)
			itFeature->second->OnActorUpdated(pWorld, pGroup->tsetActors);
	}
}

void UVirtualGroupMgr::ClearActorsOfGroup(const FName& GroupName)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
	if (pGroup)
	{
		FName4Stl Name4(GroupName);
		for (AActor* pActor : pGroup->tsetActors)
		{
			std::unordered_map<AActor*, std::set<FName4Stl>>::iterator itA2g = m_mapActorToGroups.find(pActor);
			if (itA2g == m_mapActorToGroups.end())
				continue;

			std::set<FName4Stl>::iterator itName = itA2g->second.find(Name4);
			if (itName != itA2g->second.end())
			{
				itA2g->second.erase(itName);
				if (itA2g->second.empty())
					m_mapActorToGroups.erase(itA2g);
			}
		}

		pGroup->ClearActors();

		UWorld* pWorld = (GetOuter() ? GetOuter()->GetWorld() : nullptr);
		std::map<EVirtualGroupUsage, FVirtGrpFeature*>::iterator itFeature = pGroup->mapFeatures.begin();
		for (; itFeature != pGroup->mapFeatures.end(); itFeature++)
			itFeature->second->OnActorUpdated(pWorld, pGroup->tsetActors);
	}
}

FVirtGrpFeature* UVirtualGroupMgr::AddFeatureToGroup(EVirtualGroupUsage eUsage, const FName& GroupName)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
	if (!pGroup) return nullptr;

	FVirtGrpFeature* pFeature = NewFeature(eUsage);
	if (ensureMsgf(pFeature, TEXT("UVirtualGroupMgr AddFeatureToGroup: Invalid initial usage!")))
	{
		pFeature->GroupName = GroupName;
		pGroup->AddFeature(pFeature);

		std::unordered_map<EVirtualGroupUsage, std::set<FName4Stl>>::iterator iter = m_mapUsageToGroups.find(eUsage);
		if (iter == m_mapUsageToGroups.end())
			iter = m_mapUsageToGroups.insert(std::make_pair(eUsage, std::set<FName4Stl>())).first;

		ensureMsgf(iter->second.insert(FName4Stl(GroupName)).second,
			TEXT("UVirtualGroupMgr AddFeatureToGroup: Repeatedly add feature to same group!"));
	}
	return pFeature;
}

void UVirtualGroupMgr::K2_AddFeatureToGroup(EVirtualGroupUsage eUsage, const FName& GroupName)
{
	AddFeatureToGroup(eUsage, GroupName);
}

bool UVirtualGroupMgr::GetRTDFeatureFromGroup(const FName& GroupName, FVirtGrpRTDFeature& OutFeature)
{
	FVirtGrpRTDFeature* pFeature = GetFeatureFromGroup<FVirtGrpRTDFeature>(EVirtualGroupUsage::RelativeTimeDilation, GroupName);
	if (pFeature)
	{
		OutFeature = *pFeature;
		return true;
	}
	return false;
}

void UVirtualGroupMgr::RemoveFeatureFromGroup(EVirtualGroupUsage eUsage, const FName& GroupName)
{
	FVirtualGroup* pGroup = m_tmapGroups.Find(GroupName);
	if (pGroup)
	{
		pGroup->RemoveFeatureByUsage(eUsage);

		std::unordered_map<EVirtualGroupUsage, std::set<FName4Stl>>::iterator itU2g = m_mapUsageToGroups.find(eUsage);
		if (itU2g == m_mapUsageToGroups.end())
			return;

		std::set<FName4Stl>::iterator itName = itU2g->second.find(FName4Stl(GroupName));
		if (itName != itU2g->second.end())
		{
			itU2g->second.erase(itName);
			if (itU2g->second.empty())
				m_mapUsageToGroups.erase(itU2g);
		}
	}
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
	case EVirtualGroupUsage::CentralTarget:
		return new FVirtGrpCentralFeature();
	}
	return nullptr;
}