// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/GroupCentralTargetMgr.h"

FGroupCentralInfo::FGroupCentralInfo()
	: pCentralViewTarget(nullptr)
	, bViewing(false), pCurView(nullptr)
{
	vCentralTarget = FVector(0.0f, 0.0f, 0.0f);
}