// Fill out your copyright notice in the Description page of Project Settings.


#include "Network/NetStructs.h"
#include "NetBlueprintLibrary.h"

FServerSyncInfo::FServerSyncInfo()
	: eType(EServerSyncType::Universal)
	, iCompletedNum(0)
{
}

FServerSyncInfo::FServerSyncInfo(EServerSyncType xType)
	: eType(xType), iCompletedNum(0)
{
}

bool FServerSyncInfo::IsWaitingUsage()
{
	switch (eType)
	{
	case EServerSyncType::Universal:
	case EServerSyncType::FlowFence:
		return true;
	}
	return false;
}

bool FServerSyncInfo::UpdateComplete(UObject* pWCO)
{
	iCompletedNum++;
	if (iCompletedNum >= UNetBlueprintLibrary::Net_GetPlayerCount(pWCO))
	{
		Delegate.ExecuteIfBound();
		return true;
	}
	return false;
}