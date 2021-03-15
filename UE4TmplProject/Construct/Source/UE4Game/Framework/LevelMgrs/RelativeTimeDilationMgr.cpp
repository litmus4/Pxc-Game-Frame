// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/RelativeTimeDilationMgr.h"

FTimeDilationInfo::FTimeDilationInfo()
	: fDuration(0.0f), fBlendInTime(0.0f), fBlendOutTime(0.0f)
	, fStaticDilation(0.0f), pDynamicDilation(nullptr)
	, eLevel(ERTDilationLevel::AffectActor), pAffectActor(nullptr), bIgnoreParent(false)
	, iPriority(-1)
	, fCurBlendInTime(-1.0f), fCurBlendOutTime(-1.0f), fRemainingTime(0.0f)
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