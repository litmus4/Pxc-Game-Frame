// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/GroupCentralTargetMgr.h"

FGroupCentralInfo::FGroupCentralInfo()
	: fDefaultMoveTime(-1.0f), pDefaultDynamicMover(nullptr)
	, fDefaultBlendTime(-1.0f), pCentralViewTarget(nullptr)
	, eDefaultBlendFunc(EViewTargetBlendFunction::VTBlend_Linear)
	, bDirecting(false), pCurDirect(nullptr), pLastDirect(nullptr), fCurMoveTime(0.0f)
	, bViewing(false), pCurView(nullptr)
{
	vCentralTarget = FVector(0.0f, 0.0f, 0.0f);
	vDirectTarget = FVector(0.0f, 0.0f, 0.0f);
}

void FGroupCentralInfo::Init(const FName& xGroupName)
{
	GroupName = xGroupName;
}