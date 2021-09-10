// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestState.h"
#include <unordered_map>

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/StateMachine.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "OsQuestMgr.generated.h"

class CQuestRow;

/**
 * 
 */
UCLASS()
class UE4GAME_API UOsQuestMgr : public UObject
{
	GENERATED_BODY()

public:
	typedef PxcUtil::CStateMachine<UQuestState, int32> tQuestFsm;

	struct SQuestFsmEx
	{
		bool operator==(const int32& iOtherID) const;

		tQuestFsm Fsm;
		int32 iCurStateID;
		std::vector<SQuestFsmEx> vecTributaries;
	};
	
public:
	void InitDefault();
	//void SetStatesFromSaving(/*TODOJK ConsWait*/);
	void Init();

	void Release();

	//

private:
	void AddTributaryHead(std::vector<SQuestFsmEx>& vecTributaries, UQuestState* pState, CQuestRow* pRow,
		std::unordered_map<int32, std::vector<UQuestState*>>& mapTribuHeads, int32 iLevel);

	UPROPERTY()
	TMap<int32, UQuestState*> m_tmapQuestStates;

	std::map<int32, SQuestFsmEx> m_mapQuestFsms;
};
