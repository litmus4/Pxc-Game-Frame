// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/PxcGameMode.h"
#include "NetStructs.h"
#include "Utilities/UtilityStructs.h"
#include "PxcNetGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UE4GAME_API APxcNetGameMode : public APxcGameMode
{
	GENERATED_BODY()
	
public:
	//FLAGJK_Net

	FServerSyncInfo* FindServerSyncInfo(EServerSyncType eType);

protected:
	void AddWaitingUsedServerSyncInfo(EServerSyncType eType, float fWaitTime, int32 iAlreadyNum,
		FSimpleDynamicDelexPair& DeleCompleted);
	void AddWaitingUsedServerSyncInfo(EServerSyncType eType, float fWaitTime, int32 iAlreadyNum,
		FSimpleDelegate&& DeleSucceeded, FSimpleDelegate&& DelexFailed);
	void RemoveServerSyncInfo(EServerSyncType eType);

	TMap<EServerSyncType, FServerSyncInfo> m_tmapServerSyncInfos;
};
