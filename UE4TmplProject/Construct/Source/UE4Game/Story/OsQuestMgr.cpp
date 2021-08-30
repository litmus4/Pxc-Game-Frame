// Fill out your copyright notice in the Description page of Project Settings.


#include "Story/OsQuestMgr.h"
//#include <map>

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
	std::map<int32, std::vector<UQuestState*>> mapGraphHeads;
	std::vector<CQuestRow*>::iterator iter = vecHeadRows.begin();
	for (; iter != vecHeadRows.end(); iter++)
	{
		UQuestState* pState = NewObject<UQuestState>();
		pState->Init(*iter);
		m_tmapQuestStates.Add((*iter)->m_iID, pState);

		if ((*iter)->m_bMainHeadMark)//FLAGJK ตÝน้
		{
			verify(m_mapQuestFsms.find((*iter)->m_iGraphID) == m_mapQuestFsms.end());
			SQuestFsmEx FsmEx;
			FsmEx.iCurStateID = (*iter)->m_iID;
			FsmEx.Fsm.AddState(pState);
			FsmEx.Fsm.SetState(pState, true);
			std::pair<std::unordered_map<int32, SQuestFsmEx>::iterator, bool> RetPair =
				m_mapQuestFsms.insert(std::make_pair((*iter)->m_iGraphID, FsmEx));

			std::map<int32, std::vector<UQuestState*>>::iterator itGh = mapGraphHeads.find((*iter)->m_iGraphID);
			if (itGh != mapGraphHeads.end())
			{
				std::vector<UQuestState*>::iterator itHead = itGh->second.begin();
				for (; itHead != itGh->second.end(); itHead++)
				{
					SQuestFsmEx TriFsmEx;
					TriFsmEx.iCurStateID = (*itHead)->GetQuestID();
					TriFsmEx.Fsm.AddState(*itHead);
					TriFsmEx.Fsm.SetState(*itHead, true);
					RetPair.first->second.vecTributaries.push_back(TriFsmEx);
				}
				mapGraphHeads.erase(itGh);
			}
		}
		else
		{
			std::unordered_map<int32, SQuestFsmEx>::iterator itQf = m_mapQuestFsms.find((*iter)->m_iGraphID);
			if (itQf != m_mapQuestFsms.end())
			{
				SQuestFsmEx TriFsmEx;
				TriFsmEx.iCurStateID = (*iter)->m_iID;
				TriFsmEx.Fsm.AddState(pState);
				TriFsmEx.Fsm.SetState(pState, true);
				itQf->second.vecTributaries.push_back(TriFsmEx);
			}
			else
			{
				std::map<int32, std::vector<UQuestState*>>::iterator itGh = mapGraphHeads.find((*iter)->m_iGraphID);
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

	std::map<int32, std::vector<UQuestState*>>::iterator itGh = mapGraphHeads.begin();
	for (; itGh != mapGraphHeads.end(); itGh++)
	{
		std::vector<UQuestState*>::iterator itHead = itGh->second.begin();
		bool bMain = true;
		for (; itHead != itGh->second.end(); itHead++)
		{
			if (bMain)
			{
				SQuestFsmEx FsmEx;
				FsmEx.iCurStateID = (*itHead)->GetQuestID();
				FsmEx.Fsm.AddState(*itHead);
				FsmEx.Fsm.SetState(*itHead, true);
				m_mapQuestFsms.insert(std::make_pair(itGh->first, FsmEx));
				bMain = false;
			}
			else
			{
				std::unordered_map<int32, SQuestFsmEx>::iterator itQf = m_mapQuestFsms.find(itGh->first);
				if (itQf != m_mapQuestFsms.end())
				{
					SQuestFsmEx TriFsmEx;
					TriFsmEx.iCurStateID = (*itHead)->GetQuestID();
					TriFsmEx.Fsm.AddState(*itHead);
					TriFsmEx.Fsm.SetState(*itHead, true);
					itQf->second.vecTributaries.push_back(TriFsmEx);
				}
			}
		}
	}
}

void UOsQuestMgr::Release()
{
	m_mapQuestFsms.clear();
	m_tmapQuestStates.Empty();
}