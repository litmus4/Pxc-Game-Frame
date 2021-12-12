// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Framework/PxcGameMode.h"
#include "Framework/Structs/SignatureStructs.h"
#include "NetStructs.h"
#include "Utilities/UtilityStructs.h"
#include "PxcNetGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UEGAME_API APxcNetGameMode : public APxcGameMode
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Networking)
	void MulticastUniversal(FName TypeName, UObject* pParamObject, FSharedSignature ParamSig);

	UFUNCTION(BlueprintCallable, Category = Networking)
	void MulticastUniversalSync(FName TypeName, float fWaitTime, FSimpleDynamicDelexPair DelexCompleted,
		UObject* pParamObject, FSharedSignature ParamSig);

	FServerSyncInfo* FindServerSyncInfo(EServerSyncType eType);

	bool IsFlowFenceServerReady(const FName& CheckName);
	void ReadyFlowFence(const FName& CheckName);
	void ReadyFlowFenceOnServer(const FName& CheckName, float fWaitTime, FSimpleDynamicDelexPair& DelexCompleted);
	void RemoveFlowFence(const FName& CheckName);

protected:
	void AddWaitingUsedServerSyncInfo(EServerSyncType eType, float fWaitTime, int32 iAlreadyNum,
		FSimpleDynamicDelexPair& DeleCompleted);
	void AddWaitingUsedServerSyncInfo(EServerSyncType eType, float fWaitTime, int32 iAlreadyNum,
		FSimpleDelegate&& DeleSucceeded, FSimpleDelegate&& DelexFailed);
	void RemoveServerSyncInfo(EServerSyncType eType);

	TMap<EServerSyncType, FServerSyncInfo> m_tmapServerSyncInfos;
	TMap<FName, int32> m_tmapFlowFenceSyncCache;
};
