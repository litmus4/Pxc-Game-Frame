// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/RelativeTimeDilationMgr.h"
#include "../PxcAssistantSubsystem.h"
#include "PxcUtil/IDPool.h"

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

uint32 FTimeDilationInfo::GetPartialHash() const
{
	uint32 uHash1 = 0;
	switch (eLevel)
	{
	case ERTDilationLevel::AffectGroup:
		uHash1 = GetTypeHash(AffectGroupName); break;
	case ERTDilationLevel::AffectActor:
		uHash1 = GetTypeHash(pAffectActor); break;
	}
	uint32&& uHash2 = GetTypeHash(iPriority);
	return HashCombine(uHash1, uHash2);
}

int64 FTimeDilationInfo::GetHashEx()
{
	return ((int64)eLevel << 32 | GetPartialHash());
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
	uint32&& uHash2 = GetTypeHash(iPriority);
	return ((int64)eLevel << 32 | HashCombine(uHash1, uHash2));
}

void URelativeTimeDilationMgr::Init()
{
	for (int32 i = 0; i < (int32)ERTDilationLevel::MAX; ++i)
		m_mapLevelNums.insert(std::make_pair((ERTDilationLevel)i, 0));
}

int32 URelativeTimeDilationMgr::SetGlobalDilation(float fDuration, float fBlendInTime, float fBlendOutTime,
	float fStaticDilation, UCurveFloat* pDynamicDilation, int32 iPriority, FTimeDilationEndDelegate DeleEnded)
{
	if (iPriority < 1) iPriority = 1;

	int64 lHash = FTimeDilationInfo::MakeHashEx(ERTDilationLevel::Global, NAME_None, nullptr, iPriority);
	std::map<int64, int32>::iterator iter = m_mapHashExToUids.find(lHash);
	if (iter != m_mapHashExToUids.end())
	{
		FTimeDilationData* pData = m_tmapTimeDilations.Find(iter->second);
		if (pData)
		{
			pData->DeleEnded.ExecuteIfBound(true);
			m_tmapTimeDilations.Remove(iter->second);
			m_mapHashExToUids.erase(iter);
			m_mapLevelNums.find(ERTDilationLevel::Global)->second--;
		}
	}
	check(m_mapHashExToUids.find(lHash) == m_mapHashExToUids.end());

	PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetUidPool(ASUIDPOOL::RelativeTimeDilation);
	FTimeDilationData Data;
	Data.iUid = pUidPool->Generate();
	Data.Info.InitGlobal(fDuration, fBlendInTime, fBlendOutTime, fStaticDilation, pDynamicDilation, iPriority);
	Data.DeleEnded = DeleEnded;
	m_tmapTimeDilations.Add(Data.iUid, Data);
	m_mapHashExToUids.insert(std::make_pair(lHash, Data.iUid));
	m_mapLevelNums.find(ERTDilationLevel::Global)->second++;

	return Data.iUid;
}