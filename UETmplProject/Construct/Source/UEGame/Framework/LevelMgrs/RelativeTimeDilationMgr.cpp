// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/RelativeTimeDilationMgr.h"
#include "Kismet/GameplayStatics.h"
#include "../PxcGameMode.h"
#include "../PxcAssistantSubsystem.h"
#include "VirtualGroupMgr.h"
#include "PxcUtil/IDPool.h"
#include <unordered_set>

FTimeDilationInfo::FTimeDilationInfo()
	: fDuration(0.0f), fBlendInTime(0.0f), fBlendOutTime(0.0f)
	, fStaticDilation(0.0f), pDynamicDilation(nullptr)
	, eLevel(ERTDilationLevel::AffectActor), pAffectActor(nullptr), bIgnoreParent(false)
	, iPriority(-1)
	, fCurBlendInTime(-1.0f), fCurBlendOutTime(-1.0f), fRemainingTime(0.0f)
	, fElapsedTime(0.0f), fDynamicMax(0.0f)
{
	//
}

void FTimeDilationInfo::InitGlobal(float xDuration, float xBlendInTime, float xBlendOutTime,
	float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority)
{
	eLevel = ERTDilationLevel::Global;
	AffectGroupName = NAME_None;
	pAffectActor = nullptr;
	bIgnoreParent = false;

	InitUniversal(xDuration, xBlendInTime, xBlendOutTime, xStaticDilation, xDynamicDilation, xPriority);
}

void FTimeDilationInfo::InitGroup(const FName& GroupName, float xDuration, float xBlendInTime, float xBlendOutTime,
	float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority, bool xIgnoreParent)
{
	eLevel = ERTDilationLevel::AffectGroup;
	AffectGroupName = GroupName;
	pAffectActor = nullptr;
	bIgnoreParent = xIgnoreParent;

	InitUniversal(xDuration, xBlendInTime, xBlendOutTime, xStaticDilation, xDynamicDilation, xPriority);
}

void FTimeDilationInfo::InitActor(AActor* pActor, float xDuration, float xBlendInTime, float xBlendOutTime,
	float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority, bool xIgnoreParent)
{
	eLevel = ERTDilationLevel::AffectActor;
	AffectGroupName = NAME_None;
	pAffectActor = pActor;
	bIgnoreParent = xIgnoreParent;

	InitUniversal(xDuration, xBlendInTime, xBlendOutTime, xStaticDilation, xDynamicDilation, xPriority);
}

void FTimeDilationInfo::InitUniversal(float xDuration, float xBlendInTime, float xBlendOutTime,
	float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority)
{
	fDuration = xDuration;
	fBlendInTime = FMath::Max(xBlendInTime, 0.0f);
	fBlendOutTime = FMath::Max(xBlendOutTime, 0.0f);

	fStaticDilation = xStaticDilation;
	pDynamicDilation = xDynamicDilation;
	if (IsValid(pDynamicDilation))
	{
		float fTempMin = 0.0f;
		pDynamicDilation->FloatCurve.GetTimeRange(fTempMin, fDynamicMax);
	}

	iPriority = xPriority;

	if (fDuration <= 0.0f && fDuration >= -0.5f)
		return;

	fElapsedTime = 0.0f;
	if (fRemainingTime > 0.0f)
	{
		fRemainingTime = fDuration;

		if (fCurBlendOutTime >= -0.5f)
		{
			//fCurBlendOutTime = 0.0f;
			if (xBlendInTime > 0.0f)
				fCurBlendInTime = fBlendInTime * FMath::Max(1.0f - fCurBlendOutTime / fBlendOutTime, 0.0f);
			else
				fCurBlendInTime = -1.0f;
			fCurBlendOutTime = -1.0f;
		}
	}
	else
	{
		fRemainingTime = fDuration;

		if (xBlendInTime > 0.0f)
			fCurBlendInTime = 0.0f;
	}
}

void FTimeDilationInfo::UpdateDilation(float fDeltaSeconds, float& fOutDilation)
{
	float fBaseScale = 1.0f;

	fElapsedTime += fDeltaSeconds;
	if (fRemainingTime > 0)
	{
		fRemainingTime -= fDeltaSeconds;
		fRemainingTime = FMath::Max(fRemainingTime, 0.0f);
	}
	if (fCurBlendInTime >= -0.5f)
		fCurBlendInTime += fDeltaSeconds;
	if (fCurBlendOutTime >= -0.5f)
		fCurBlendOutTime += fDeltaSeconds;

	bool bFinished = false;
	if (fRemainingTime <= 0.0f && fRemainingTime >= -0.5f)//完成!
		bFinished = true;
	else if (fRemainingTime < -0.5f)
	{/*无限*/}
	else if (fRemainingTime < fBlendOutTime)//开始混出
		fCurBlendOutTime = fBlendOutTime - fRemainingTime;

	if (fCurBlendInTime >= -0.5f)
	{
		if (fCurBlendInTime > fBlendInTime)
			fCurBlendInTime = -1.0f;
	}
	if (fCurBlendOutTime >= -0.5f)
	{
		if (fCurBlendOutTime > fBlendOutTime)
		{
			fCurBlendOutTime = fBlendOutTime;
			bFinished = true;
		}
	}

	if (!bFinished)
	{
		float fBlendInWeight = (fCurBlendInTime >= -0.5f) ? (fCurBlendInTime / fBlendInTime) : 1.0f;
		float fBlendOutWeight = (fCurBlendOutTime >= -0.5f) ? (1.f - fCurBlendOutTime / fBlendOutTime) : 1.0f;
		float fCurBlendWeight = FMath::Min(fBlendInWeight, fBlendOutWeight);
		float fDilationScale = fBaseScale * fCurBlendWeight;

		if (fDilationScale > 0.0f)
		{
			float fTargetDilation = fStaticDilation;
			if (IsValid(pDynamicDilation))
			{
				float fRealDuration = (fDuration >= -0.5f ? fDuration : fDynamicMax);
				fTargetDilation = pDynamicDilation->GetFloatValue(FMath::Fmod(fElapsedTime, fRealDuration));
			}
			fOutDilation = FMath::GetMappedRangeValueClamped(FVector2D(0, 1), FVector2D(1.0, fTargetDilation), fDilationScale);
		}
	}
	else
		fOutDilation = 1.0f;
}

bool FTimeDilationInfo::IsFinished() const
{
	if (eLevel == ERTDilationLevel::AffectActor && !IsValid(pAffectActor))
		return true;
	return (fRemainingTime <= 0.0f && !IsLooping());
}

bool FTimeDilationInfo::IsLooping() const
{
	return (fDuration < -0.5f);
}

uint32 GetTypeHash(const FTimeDilationInfo& Info)
{
	uint32&& uHash1 = GetTypeHash(Info.eLevel);
	uint32&& uHash2 = Info.GetPartialHash();
	return HashCombine(uHash1, uHash2);
}

uint32 FTimeDilationInfo::GetPartialHash(bool bNoPriority) const
{
	uint32 uHash1 = 0;
	switch (eLevel)
	{
	case ERTDilationLevel::AffectGroup:
		uHash1 = GetTypeHash(AffectGroupName); break;
	case ERTDilationLevel::AffectActor:
		uHash1 = GetTypeHash(pAffectActor); break;
	}
	if (bNoPriority) return uHash1;
	uint32&& uHash2 = GetTypeHash(iPriority);
	return HashCombine(uHash1, uHash2);
}

int64 FTimeDilationInfo::GetHashEx(bool bNoPriority)
{
	return ((int64)eLevel << 32 | GetPartialHash(bNoPriority));
}

int64 FTimeDilationInfo::MakeHashEx(ERTDilationLevel eLevel, const FName& GroupName, AActor* pActor, int32 iPriority)
{
	uint32 uHash1 = 0;
	switch (eLevel)
	{
	case ERTDilationLevel::AffectGroup:
		uHash1 = GetTypeHash(GroupName); break;
	case ERTDilationLevel::AffectActor:
		uHash1 = GetTypeHash(pActor); break;
	}
	if (iPriority == 0) return ((int64)eLevel << 32 | uHash1);
	uint32&& uHash2 = GetTypeHash(iPriority);
	return ((int64)eLevel << 32 | HashCombine(uHash1, uHash2));
}

int32 URelativeTimeDilationMgr::SetGlobalDilation(float fDuration, float fBlendInTime, float fBlendOutTime,
	float fStaticDilation, UCurveFloat* pDynamicDilation, int32 iPriority, FTimeDilationEndDelegate DeleEnded)
{
	if (iPriority < 1) iPriority = 1;

	int64 lHashEx = FTimeDilationInfo::MakeHashEx(ERTDilationLevel::Global, NAME_None, nullptr, iPriority);
	FTimeDilationData* pData = nullptr;

	std::map<int64, int32>::iterator itH2u = m_mapHashExToUids.find(lHashEx);
	if (itH2u != m_mapHashExToUids.end())
	{
		pData = m_tmapTimeDilations.Find(itH2u->second);
		if (pData)
		{
			pData->DeleEnded.ExecuteIfBound(true);//TODOJK EventCenter
			m_tmapTimeDilations.Remove(itH2u->second);
			m_mapHashExToUids.erase(itH2u);
		}
	}
	check(m_mapHashExToUids.find(lHashEx) == m_mapHashExToUids.end());

	PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetUidPool(ASUIDPOOL::RelativeTimeDilation);
	FTimeDilationData Data;
	Data.iUid = pUidPool->Generate();
	Data.Info.InitGlobal(fDuration, fBlendInTime, fBlendOutTime, fStaticDilation, pDynamicDilation, iPriority);
	Data.DeleEnded = DeleEnded;

	m_tmapTimeDilations.Add(Data.iUid, Data);
	m_mapHashExToUids.insert(std::make_pair(lHashEx, Data.iUid));
	if (pData == nullptr)
	{
		int64 lHashExnp = Data.Info.GetHashEx(true);
		std::unordered_map<int64, int32>::iterator itHn = m_mapHashExnpNums.find(lHashExnp);
		if (itHn == m_mapHashExnpNums.end())
			itHn = m_mapHashExnpNums.insert(std::make_pair(lHashExnp, 0)).first;
		itHn->second++;
	}

	return Data.iUid;
}

int32 URelativeTimeDilationMgr::SetGroupDilation(const FName& GroupName, float fDuration, float fBlendInTime, float fBlendOutTime,
	float fStaticDilation, UCurveFloat* pDynamicDilation, int32 iPriority, bool bIgnoreParent, FTimeDilationEndDelegate DeleEnded)
{
	if (!m_pGM) m_pGM = CastChecked<APxcGameMode>(GetOuter());
	UVirtualGroupMgr* pManager = m_pGM->GetVirtualGroupMgr();
	check(pManager);
	if (!pManager->GetFeatureFromGroup<FVirtGrpRTDFeature>(EVirtualGroupUsage::RelativeTimeDilation, GroupName))
		return -1;
	if (iPriority < 1) iPriority = 1;

	int64 lHashEx = FTimeDilationInfo::MakeHashEx(ERTDilationLevel::AffectGroup, GroupName, nullptr, iPriority);
	FTimeDilationData* pData = nullptr;

	std::map<int64, int32>::iterator itH2u = m_mapHashExToUids.find(lHashEx);
	if (itH2u != m_mapHashExToUids.end())
	{
		pData = m_tmapTimeDilations.Find(itH2u->second);
		if (pData)
		{
			pData->DeleEnded.ExecuteIfBound(true);//TODOJK EventCenter
			m_tmapTimeDilations.Remove(itH2u->second);
			m_mapHashExToUids.erase(itH2u);
		}
	}
	check(m_mapHashExToUids.find(lHashEx) == m_mapHashExToUids.end());

	PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetUidPool(ASUIDPOOL::RelativeTimeDilation);
	FTimeDilationData Data;
	Data.iUid = pUidPool->Generate();
	Data.Info.InitGroup(GroupName, fDuration, fBlendInTime, fBlendOutTime, fStaticDilation, pDynamicDilation, iPriority, bIgnoreParent);
	Data.DeleEnded = DeleEnded;

	m_tmapTimeDilations.Add(Data.iUid, Data);
	m_mapHashExToUids.insert(std::make_pair(lHashEx, Data.iUid));
	if (pData == nullptr)
	{
		int64 lHashExnp = Data.Info.GetHashEx(true);
		std::unordered_map<int64, int32>::iterator itHn = m_mapHashExnpNums.find(lHashExnp);
		if (itHn == m_mapHashExnpNums.end())
			itHn = m_mapHashExnpNums.insert(std::make_pair(lHashExnp, 0)).first;
		itHn->second++;
	}

	return Data.iUid;
}

int32 URelativeTimeDilationMgr::SetActorDilation(AActor* pActor, float fDuration, float fBlendInTime, float fBlendOutTime,
	float fStaticDilation, UCurveFloat* pDynamicDilation, int32 iPriority, bool bIgnoreParent, FTimeDilationEndDelegate DeleEnded)
{
	if (!m_pGM) m_pGM = CastChecked<APxcGameMode>(GetOuter());
	if (!IsValid(pActor) || pActor == m_pGM)
		return -1;
	if (iPriority < 1) iPriority = 1;

	int64 lHashEx = FTimeDilationInfo::MakeHashEx(ERTDilationLevel::AffectActor, NAME_None, pActor, iPriority);
	FTimeDilationData* pData = nullptr;

	std::map<int64, int32>::iterator itH2u = m_mapHashExToUids.find(lHashEx);
	if (itH2u != m_mapHashExToUids.end())
	{
		pData = m_tmapTimeDilations.Find(itH2u->second);
		if (pData)
		{
			pData->DeleEnded.ExecuteIfBound(true);//TODOJK EventCenter
			m_tmapTimeDilations.Remove(itH2u->second);
			m_mapHashExToUids.erase(itH2u);
		}
	}
	check(m_mapHashExToUids.find(lHashEx) == m_mapHashExToUids.end());

	PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetUidPool(ASUIDPOOL::RelativeTimeDilation);
	FTimeDilationData Data;
	Data.iUid = pUidPool->Generate();
	Data.Info.InitActor(pActor, fDuration, fBlendInTime, fBlendOutTime, fStaticDilation, pDynamicDilation, iPriority, bIgnoreParent);
	Data.DeleEnded = DeleEnded;

	m_tmapTimeDilations.Add(Data.iUid, Data);
	m_mapHashExToUids.insert(std::make_pair(lHashEx, Data.iUid));
	if (pData == nullptr)
	{
		int64 lHashExnp = Data.Info.GetHashEx(true);
		std::unordered_map<int64, int32>::iterator itHn = m_mapHashExnpNums.find(lHashExnp);
		if (itHn == m_mapHashExnpNums.end())
			itHn = m_mapHashExnpNums.insert(std::make_pair(lHashExnp, 0)).first;
		itHn->second++;
	}

	return Data.iUid;
}

void URelativeTimeDilationMgr::ResetDilationByUid(int32 iUid, bool bCanceled, bool bDisableCallback)
{
	if (iUid < 0) return;
	if (!m_pGM) m_pGM = CastChecked<APxcGameMode>(GetOuter());

	FTimeDilationData* pData = m_tmapTimeDilations.Find(iUid);
	if (!pData) return;

	if (!bDisableCallback)
	{
		pData->DeleEnded.ExecuteIfBound(bCanceled);
		//TODOJK EventCenter
	}

	m_mapHashExToUids.erase(pData->Info.GetHashEx());
	std::unordered_map<int64, int32>::iterator itHn = m_mapHashExnpNums.find(pData->Info.GetHashEx(true));
	if (itHn != m_mapHashExnpNums.end())
	{
		itHn->second--;
		if (itHn->second <= 0)
		{
			switch (pData->Info.eLevel)
			{
			case ERTDilationLevel::Global:
				UGameplayStatics::SetGlobalTimeDilation(this, 1.0f);
				break;
			case ERTDilationLevel::AffectGroup:
			{
				UVirtualGroupMgr* pManager = m_pGM->GetVirtualGroupMgr();
				check(pManager);
				FVirtualGroup* pGroup = pManager->GetGroup(pData->Info.AffectGroupName);
				if (!pGroup) break;

				FVirtGrpRTDFeature* pFeature =
					pGroup->GetFeatureByUsage<FVirtGrpRTDFeature>(EVirtualGroupUsage::RelativeTimeDilation);
				if (!pFeature) break;
				pFeature->fTimeDilation = 1.0f;

				for (AActor* pActor : pGroup->tsetActors)
				{
					if (!IsValid(pActor)) continue;

					if (!pFeature->bRuntimeActorExclusiveMark)
					{
						if (pManager->ForEachGroupWithActor<FVirtGrpRTDFeature>(pActor, EVirtualGroupUsage::RelativeTimeDilation,
							[pGroup, this](FVirtualGroup* pMultiGroup, FVirtGrpRTDFeature* pMultiFeature)->bool {
								if (pMultiGroup == pGroup) return false;
								int64 lHashExnp = FTimeDilationInfo::MakeHashEx(ERTDilationLevel::AffectGroup, pMultiGroup->Name, nullptr);
								return (m_mapHashExnpNums.find(lHashExnp) != m_mapHashExnpNums.end());
							})) continue;
					}

					int64 lHashExnp = FTimeDilationInfo::MakeHashEx(ERTDilationLevel::AffectActor, NAME_None, pActor);
					if (m_mapHashExnpNums.find(lHashExnp) != m_mapHashExnpNums.end())
						continue;

					pActor->CustomTimeDilation = 1.0f;
				}
				break;
			}
			case ERTDilationLevel::AffectActor:
				if (!IsValid(pData->Info.pAffectActor))
					break;

				UVirtualGroupMgr* pManager = m_pGM->GetVirtualGroupMgr();
				check(pManager);
				if (pManager->ForEachGroupWithActor<FVirtGrpRTDFeature>(pData->Info.pAffectActor,
					EVirtualGroupUsage::RelativeTimeDilation, [this](FVirtualGroup* pMultiGroup, FVirtGrpRTDFeature* pMultiFeature)->bool {
						int64 lHashExnp = FTimeDilationInfo::MakeHashEx(ERTDilationLevel::AffectGroup, pMultiGroup->Name, nullptr);
						return (m_mapHashExnpNums.find(lHashExnp) != m_mapHashExnpNums.end());
					})) break;

				pData->Info.pAffectActor->CustomTimeDilation = 1.0f;
				break;
			}
			m_mapHashExnpNums.erase(itHn);
		}
	}

	m_tmapTimeDilations.Remove(iUid);
}

void URelativeTimeDilationMgr::ResetDilationByGroupName(const FName& GroupName)
{
	std::vector<int32> vecResets;
	for (auto& Pair : m_tmapTimeDilations)
	{
		if (Pair.Value.Info.eLevel == ERTDilationLevel::AffectGroup && Pair.Value.Info.AffectGroupName == GroupName)
			vecResets.push_back(Pair.Key);
	}

	std::vector<int32>::iterator iter = vecResets.begin();
	for (; iter != vecResets.end(); iter++)
		ResetDilationByUid(*iter);
}

void URelativeTimeDilationMgr::ResetDilationByActor(AActor* pActor)
{
	if (!pActor) return;

	std::vector<int32> vecResets;
	for (auto& Pair : m_tmapTimeDilations)
	{
		if (Pair.Value.Info.eLevel == ERTDilationLevel::AffectActor && Pair.Value.Info.pAffectActor == pActor)
			vecResets.push_back(Pair.Key);
	}

	std::vector<int32>::iterator iter = vecResets.begin();
	for (; iter != vecResets.end(); iter++)
		ResetDilationByUid(*iter);
}

void URelativeTimeDilationMgr::Tick(float fDeltaSeconds)
{
	if (!m_pGM) m_pGM = CastChecked<APxcGameMode>(GetOuter());
	float fDt = (fDeltaSeconds / m_pGM->GetWorldSettings()->TimeDilation) / m_pGM->CustomTimeDilation;
	UVirtualGroupMgr* pManager = m_pGM->GetVirtualGroupMgr();
	check(pManager);

	SActorPreferred BestGlobal;
	BestGlobal.iPriority = -1;
	BestGlobal.fDilation = 1.0f;
	TMap<FName, SGroupPreferred> tmapBestGroups;
	TMap<AActor*, SActorPreferred> tmapBestActors;
	std::vector<int32> vecFinished;

	std::map<int64, int32>::iterator itH2u = m_mapHashExToUids.begin();
	for (; itH2u != m_mapHashExToUids.end(); itH2u++)
	{
		FTimeDilationData* pData = m_tmapTimeDilations.Find(itH2u->second);
		if (!pData) continue;

		FTimeDilationInfo& Info = pData->Info;
		float fCalcDilation = 1.0f;
		Info.UpdateDilation(fDt, fCalcDilation);
		if (Info.IsFinished())
		{
			vecFinished.push_back(pData->iUid);
			continue;
		}

		switch (Info.eLevel)
		{
		case ERTDilationLevel::Global:
			if (Info.iPriority > BestGlobal.iPriority)
			{
				BestGlobal.iPriority = Info.iPriority;
				BestGlobal.fDilation = fCalcDilation;
			}
			break;
		case ERTDilationLevel::AffectGroup:
		{
			FVirtualGroup* pGroup = pManager->GetGroup(Info.AffectGroupName);
			if (!pGroup) break;
			FVirtGrpRTDFeature* pFeature = pGroup->GetFeatureByUsage<FVirtGrpRTDFeature>(EVirtualGroupUsage::RelativeTimeDilation);
			if (!pFeature) break;
			if (Info.bIgnoreParent)//bRuntimeActorExclusiveMark为true时才能忽略父级
				fCalcDilation /= m_pGM->GetWorldSettings()->TimeDilation;

			SGroupPreferred* pBest = tmapBestGroups.Find(Info.AffectGroupName);
			if (!pBest)
			{
				tmapBestGroups.Add(Info.AffectGroupName, SGroupPreferred{ Info.iPriority, &pGroup->tsetActors });
				pFeature->fTimeDilation = fCalcDilation;
			}
			else if (Info.iPriority > pBest->iPriority)
			{
				pBest->iPriority = Info.iPriority;
				pFeature->fTimeDilation = fCalcDilation;
			}
			break;
		}
		case ERTDilationLevel::AffectActor:
			check(IsValid(Info.pAffectActor));
			if (Info.bIgnoreParent)
				fCalcDilation /= m_pGM->GetWorldSettings()->TimeDilation;

			SActorPreferred* pBest = tmapBestActors.Find(Info.pAffectActor);
			if (!pBest)
			{
				SActorPreferred NewBest;
				NewBest.iPriority = Info.iPriority;
				NewBest.fDilationInGroup = 1.0f;

				if (!Info.bIgnoreParent)
				{
					pManager->ForEachGroupWithActor<FVirtGrpRTDFeature>(Info.pAffectActor, EVirtualGroupUsage::RelativeTimeDilation,
						[&NewBest](FVirtualGroup* pMultiGroup, FVirtGrpRTDFeature* pMultiFeature)->bool {
							NewBest.fDilationInGroup *= pMultiFeature->fTimeDilation;
							return pMultiFeature->bRuntimeActorExclusiveMark;
						});
					NewBest.fDilation = fCalcDilation * NewBest.fDilationInGroup;
				}
				else
					NewBest.fDilation = fCalcDilation;

				tmapBestActors.Add(Info.pAffectActor, NewBest);
			}
			else if (Info.iPriority > pBest->iPriority)
			{
				pBest->iPriority = Info.iPriority;
				if (!Info.bIgnoreParent)
					pBest->fDilation = fCalcDilation * pBest->fDilationInGroup;
				else
					pBest->fDilation = fCalcDilation;
			}
			break;
		}
	}

	for (auto& ActorPair : tmapBestActors)
	{
		check(IsValid(ActorPair.Key));
		ActorPair.Key->CustomTimeDilation = ActorPair.Value.fDilation;
	}

	std::unordered_set<AActor*> setAppliedGrpActors;
	for (auto& GroupPair : tmapBestGroups)
	{
		FVirtGrpRTDFeature* pFeature = pManager->GetFeatureFromGroup<FVirtGrpRTDFeature>(
			EVirtualGroupUsage::RelativeTimeDilation, GroupPair.Key);
		check(pFeature);
		for (AActor* pActor : *GroupPair.Value.ptsetActors)
		{
			if (setAppliedGrpActors.find(pActor) != setAppliedGrpActors.end())
				continue;
			if (tmapBestActors.Contains(pActor))
				continue;

			check(IsValid(pActor));
			pActor->CustomTimeDilation = pFeature->fTimeDilation;
			setAppliedGrpActors.insert(pActor);
		}
	}

	if (BestGlobal.iPriority > 0)
		UGameplayStatics::SetGlobalTimeDilation(this, BestGlobal.fDilation);

	std::vector<int32>::iterator itFinish = vecFinished.begin();
	for (; itFinish != vecFinished.end(); itFinish++)
		ResetDilationByUid(*itFinish, false);
}

void URelativeTimeDilationMgr::Release()
{
	for (auto& Pair : m_tmapTimeDilations)
		Pair.Value.DeleEnded.ExecuteIfBound(true);
}