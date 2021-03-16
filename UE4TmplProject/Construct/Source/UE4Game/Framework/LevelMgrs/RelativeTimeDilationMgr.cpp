// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/RelativeTimeDilationMgr.h"

FTimeDilationInfo::FTimeDilationInfo()
	: fDuration(0.0f), fBlendInTime(0.0f), fBlendOutTime(0.0f)
	, fStaticDilation(0.0f), pDynamicDilation(nullptr)
	, eLevel(ERTDilationLevel::AffectActor), pAffectActor(nullptr), bIgnoreParent(false)
	, iPriority(-1)
	, fCurBlendInTime(-1.0f), fCurBlendOutTime(-1.0f), fRemainingTime(0.0f), fElapsedTime(0.0f)
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
				fTargetDilation = pDynamicDilation->GetFloatValue(FMath::Fmod(fElapsedTime, fDuration));
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