// Fill out your copyright notice in the Description page of Project Settings.


#include "Story/OsQuestMgr.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/StoryTable/StoryTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

void UOsQuestMgr::InitDefault()
{
	//
}

void UOsQuestMgr::Init()
{
	std::vector<CQuestRow*> vecHeadRows;
	CStoryTableCenter::GetInstance()->GetNextQuestRows(-1, vecHeadRows);
	std::unordered_map<int32, std::vector<UQuestState*>> mapGraphHeads;
	std::vector<CQuestRow*>::iterator iter = vecHeadRows.begin();
	for (; iter != vecHeadRows.end(); iter++)
	{
		UQuestState* pState = NewObject<UQuestState>();
		pState->Init(*iter);
		m_tmapQuestStates.Add((*iter)->m_iID, pState);

		if ((*iter)->m_bMainHeadMark)
		{
			verify(m_mapQuestFsms.find((*iter)->m_iGraphID) == m_mapQuestFsms.end());
			SQuestFsmEx FsmEx;
			FsmEx.iCurStateID = (*iter)->m_iID;
			//FsmEx.Fsm
			m_mapQuestFsms.insert(std::make_pair((*iter)->m_iGraphID, FsmEx));
			//FLAGJK
		}
		else
		{
			std::unordered_map<int32, std::vector<UQuestState*>>::iterator itGh = mapGraphHeads.find((*iter)->m_iGraphID);
			if (itGh == mapGraphHeads.end())
			{
				std::vector<UQuestState*> vecHeads;
				vecHeads.push_back(pState);
				mapGraphHeads.insert(std::make_pair((*iter)->m_iGraphID, vecHeads));
			}
			else
				itGh->second.push_back(pState);
		}
	}
}

void UOsQuestMgr::Release()
{
	m_mapQuestFsms.clear();
	m_tmapQuestStates.Empty();
}