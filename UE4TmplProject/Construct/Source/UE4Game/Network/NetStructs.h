// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrivateDefinitions/NetDef.h"
#include "NetStructs.generated.h"

USTRUCT()
struct FServerSyncInfo
{
	GENERATED_BODY()
public:
	FServerSyncInfo();
	FServerSyncInfo(EServerSyncType xType);

	bool IsWaitingUsage();
	bool UpdateComplete(UObject* pWCO);

	EServerSyncType eType;
	int32 iCompletedNum;
	FTimerHandle Timer;
	FSimpleDelegate Delegate;
};