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
class UEGAME_API UOsQuestMgr : public UObject
{
	GENERATED_BODY()

public:
	typedef PxcUtil::CStateMachine<UQuestState, int32> tQuestFsm;

	struct SQuestFsmEx
	{
		bool operator==(const int32& iOtherID) const;

		tQuestFsm Fsm;
		int32 iCurStateID;
		int32 iHeadStateID;
		std::vector<SQuestFsmEx> vecTributaries;
	};

	//*¥Êµµ¡Ÿ ±
	struct SSavedHead
	{
		int32 iID;
		int32 iHeadID;
		bool bInit;
		std::map<int32, SSavedHead> mapSavedTribuHeads;
	};
	//*/
	
public:
	void InitDefault();
	//void SetStatesFromSaving(/*TODOJK ConsWait*/);
	void Init();

	void Release();

	//

private:
	CQuestRow* NewBeginningRow(CQuestRow* pFirstRow);
	void AddTributaryHead(std::vector<SQuestFsmEx>& vecTributaries, UQuestState* pState, CQuestRow* pRow,
		std::unordered_map<int32, std::vector<UQuestState*>>& mapTribuHeads,
		std::map<int32, SSavedHead>* pmapSavedTribuHeads, int32 iLevel);
	void SupplyExtraTributaryHeads(std::vector<SQuestFsmEx>& vecTributaries,
		std::map<int32, SSavedHead>& mapSavedTribuHeads);
	void ExtendSubNext(SQuestFsmEx& FsmEx, UQuestState* pPrevState, CQuestRow* pRow,
		std::vector<int>::iterator itFirstNsp, int32 iLevel, int32 iTriLevel, UQuestState** ppConfState);
	UQuestState* FsmLinkState(SQuestFsmEx& FsmEx, UQuestState** ppPrevState, CQuestRow* pRow,
		bool bSetPrev = false, bool bRetConfluent = false);
	void ExtendTributaries(std::vector<SQuestFsmEx>& vecTributaries, int32 iLevel);

	UPROPERTY()
	TMap<int32, UQuestState*> m_tmapQuestStates;

	std::map<int32, SQuestFsmEx> m_mapQuestFsms;
	std::map<int32, CQuestRow*> m_mapBeginningRows;

	//*¥Êµµ¡Ÿ ±
	std::map<int32, SSavedHead> m_mapSavedHeads;
	//*/
};
