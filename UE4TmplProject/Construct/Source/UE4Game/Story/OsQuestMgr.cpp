// Fill out your copyright notice in the Description page of Project Settings.


#include "Story/OsQuestMgr.h"
//#include <map>

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/StoryTable/StoryTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

bool UOsQuestMgr::SQuestFsmEx::operator==(const int32& iOtherID) const
{
	return (iCurStateID == iOtherID);
}

void UOsQuestMgr::InitDefault()
{
	//
}

void UOsQuestMgr::Init()
{
	std::vector<CQuestRow*> vecHeadRows;
	CStoryTableCenter::GetInstance()->GetNextQuestRows(-1, vecHeadRows);
	std::map<int32, std::vector<UQuestState*>> mapGraphHeads;
	std::unordered_map<int32, std::vector<UQuestState*>> mapTribuHeads;

	std::vector<CQuestRow*>::iterator iter = vecHeadRows.begin();
	for (; iter != vecHeadRows.end(); iter++)
	{
		UQuestState* pState = NewObject<UQuestState>();
		pState->Init(*iter);
		m_tmapQuestStates.Add((*iter)->m_iID, pState);

		if ((*iter)->m_iHeadLevel == 0)
		{
			verify(m_mapQuestFsms.find((*iter)->m_iGraphID) == m_mapQuestFsms.end());
			SQuestFsmEx FsmEx;
			FsmEx.iCurStateID = (*iter)->m_iID;
			FsmEx.Fsm.AddState(pState);
			FsmEx.Fsm.SetState(pState, true);
			std::pair<std::map<int32, SQuestFsmEx>::iterator, bool> RetPair =
				m_mapQuestFsms.insert(std::make_pair((*iter)->m_iGraphID, FsmEx));

			std::map<int32, std::vector<UQuestState*>>::iterator itGh = mapGraphHeads.find((*iter)->m_iGraphID);
			if (itGh != mapGraphHeads.end())
			{
				std::vector<UQuestState*>::iterator itHead = itGh->second.begin();
				for (; itHead != itGh->second.end(); itHead++)
					AddTributaryHead(RetPair.first->second.vecTributaries, *itHead, (*itHead)->GetQuestRow(), mapTribuHeads, 1);
				mapGraphHeads.erase(itGh);
			}
		}
		else
		{
			std::map<int32, SQuestFsmEx>::iterator itQf = m_mapQuestFsms.find((*iter)->m_iGraphID);
			if (itQf == m_mapQuestFsms.end())
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
			else
				AddTributaryHead(itQf->second.vecTributaries, pState, *iter, mapTribuHeads, 1);
		}
	}

	verify(mapTribuHeads.empty());//二级支流必须有头
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
				std::map<int32, SQuestFsmEx>::iterator itQf = m_mapQuestFsms.find(itGh->first);
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

	std::map<int32, SQuestFsmEx>::iterator itQf = m_mapQuestFsms.begin();
	for (; itQf != m_mapQuestFsms.end(); itQf++)
	{
		UQuestState* pPrevState = itQf->second.Fsm.GetCurrentState();
		while (pPrevState)
		{
			std::vector<CQuestRow*> vecNextRows;
			CStoryTableCenter::GetInstance()->GetNextQuestRows(pPrevState->GetQuestID(), vecNextRows);
			std::vector<CQuestRow*>::iterator itNext = vecNextRows.begin();
			for (; itNext != vecNextRows.end(); itNext++)
			{
				UQuestState* pState = nullptr;

				std::vector<int>::iterator itNsp =//Next Sub Prev
					std::find((*itNext)->m_vecSubPrevQuestIDs.begin(), (*itNext)->m_vecSubPrevQuestIDs.end(),
					pPrevState->GetQuestID());
				//这个it在下句if的外边，是因为多Next递归时同样需要此判断，但存在“二\二”的情况需要选择主干，保证格式统一
				if (vecNextRows.size() == 1)
				{
					if (itNsp == (*itNext)->m_vecSubPrevQuestIDs.end())
					{
						pState = NewObject<UQuestState>();
						pState->Init(*itNext);
					}
					else
						verify(false);//主干的交汇处不能是SubPrev
				}
				else
				{
					pState = pState/*TODOJK 递归结尾确定主干*/;
				}

				if (pState)
				{
					if (vecNextRows.size() == 1)
					{
						m_tmapQuestStates.Add((*itNext)->m_iID, pState);
						itQf->second.Fsm.AddState(pState);
						itQf->second.Fsm.AddTransfer(pPrevState, (*itNext)->m_iID, pState);
					}
					pPrevState = pState;
				}
				else
					pPrevState = nullptr;
			}
		}

		//
	}
}

void UOsQuestMgr::Release()
{
	m_mapQuestFsms.clear();
	m_tmapQuestStates.Empty();
}

void UOsQuestMgr::AddTributaryHead(std::vector<SQuestFsmEx>& vecTributaries, UQuestState* pState, CQuestRow* pRow,
	std::unordered_map<int32, std::vector<UQuestState*>>& mapTribuHeads, int32 iLevel)
{
	if (pRow->m_iHeadLevel == iLevel)
	{
		SQuestFsmEx FsmEx;
		FsmEx.iCurStateID = pRow->m_iID;
		FsmEx.Fsm.AddState(pState);
		FsmEx.Fsm.SetState(pState, true);
		vecTributaries.push_back(FsmEx);
		SQuestFsmEx& FsmPushed = vecTributaries[vecTributaries.size() - 1];

		std::unordered_map<int32, std::vector<UQuestState*>>::iterator itTh = mapTribuHeads.find(pRow->m_iID);
		if (itTh != mapTribuHeads.end())
		{
			std::vector<UQuestState*>::iterator itHead = itTh->second.begin();
			for (; itHead != itTh->second.end(); itHead++)
				AddTributaryHead(FsmPushed.vecTributaries, *itHead, (*itHead)->GetQuestRow(), mapTribuHeads, iLevel + 1);
			mapTribuHeads.erase(itTh);
		}
	}
	else
	{
		CQuestRow* pUpRow = CStoryTableCenter::GetInstance()->GetQuestRow(pRow->m_iParentHeadQuestID);
		int32 iUpLevel = pRow->m_iHeadLevel - 1;
		while (iUpLevel > iLevel)
		{
			pUpRow = CStoryTableCenter::GetInstance()->GetQuestRow(pUpRow->m_iParentHeadQuestID);
			iUpLevel--;
		}

		std::vector<SQuestFsmEx>::iterator iter = std::find(vecTributaries.begin(), vecTributaries.end(), pUpRow->m_iID);
		if (iter == vecTributaries.end())
		{
			std::unordered_map<int32, std::vector<UQuestState*>>::iterator itTh = mapTribuHeads.find(pUpRow->m_iID);
			if (itTh == mapTribuHeads.end())
			{
				std::vector<UQuestState*> vecHeads;
				vecHeads.push_back(pState);
				mapTribuHeads.insert(std::make_pair(pUpRow->m_iID, vecHeads));
			}
			else
				itTh->second.push_back(pState);
		}
		else
			AddTributaryHead(iter->vecTributaries, pState, pRow, mapTribuHeads, iLevel + 1);
	}
}

//TODOJK 返回值应该放到参数中，因为需要最终决定，否则外边循环的pPrevState就会错误
UQuestState* UOsQuestMgr::ExtendSubNext(SQuestFsmEx& FsmEx, UQuestState* pPrevState, CQuestRow* pRow, int32 iLevel, int32 iTriLevel)
{
	std::vector<int>::iterator itNsp =//Next Sub Prev
		std::find(pRow->m_vecSubPrevQuestIDs.begin(), pRow->m_vecSubPrevQuestIDs.end(),
		pPrevState->GetQuestID());
	if (itNsp == pRow->m_vecSubPrevQuestIDs.end())
	{
		UQuestState* pState = NewObject<UQuestState>();
		pState->Init(pRow);
		m_tmapQuestStates.Add(pRow->m_iID, pState);//TODOJK 这里应该改为DO模式

		FsmEx.Fsm.AddState(pState);
		FsmEx.Fsm.AddTransfer(pPrevState, pRow->m_iID, pState);

		pPrevState = pState;
	}
	else if (iTriLevel > 0)
		pPrevState = nullptr;//"二\二"上支流下干流
	else if (iLevel > 1)
		pPrevState = nullptr;
	else
		verify(false);//1级分干的第一个依然属于主干，主干的交汇处不能是SubPrev

	while (pPrevState)
	{
		std::vector<CQuestRow*> vecNextRows;
		CStoryTableCenter::GetInstance()->GetNextQuestRows(pPrevState->GetQuestID(), vecNextRows);
		std::vector<CQuestRow*>::iterator itNext = vecNextRows.begin();
		for (; itNext != vecNextRows.end(); itNext++)
		{
			UQuestState* pState = nullptr;

			itNsp = std::find((*itNext)->m_vecSubPrevQuestIDs.begin(),//Next Sub Prev
				(*itNext)->m_vecSubPrevQuestIDs.end(), pPrevState->GetQuestID());
			if (vecNextRows.size() == 1)
			{
				//Sub不为空则为交汇点
				//Normal: Sub有-分干到头了，Sub无-主干到头了
				//FreshLake: Sub有-我是支流的分干到头了，Sub无-正常
				//Combo: Sub有-若支流则支流的分干到头了 若干流则分干到头了，Sub无-若支流则支流的主干到头了 若干流则主干到头了
				//Sub为空则正常
				if (itNsp == (*itNext)->m_vecSubPrevQuestIDs.end())
				{
					switch ((*itNext)->m_ePrevConfType)
					{
					case StoryDef::EPrevConfluentType::EPrevNormal:
						//DO并置空pPrevState
						break;
					case StoryDef::EPrevConfluentType::EPrevFreshLake:
						if (iTriLevel > 0)
						{
							//DO并置空pPrevState(是不是支流的主干暂无法确定)
						}
						else
							verify(false);
						break;
					case StoryDef::EPrevConfluentType::EPrevCombo:
						if (iTriLevel > 0)
						{
							//DO并置空pPrevState(是不是支流的主干暂无法确定)
						}
						else
						{
							//DO并置空pPrevState
						}
						break;
					}
				}
				else if (!(*itNext)->m_vecSubPrevQuestIDs.empty())
				{
					switch ((*itNext)->m_ePrevConfType)
					{
					case StoryDef::EPrevConfluentType::EPrevNormal:
						//DO并置空pPrevState，确定返回值
						break;
					case StoryDef::EPrevConfluentType::EPrevFreshLake:
						//DO并赋值pPrevState
						break;
					case StoryDef::EPrevConfluentType::EPrevCombo:
						if (iTriLevel > 0)
						{
							//DO并置空pPrevState，确定返回值
						}
						else
						{
							//DO并置空pPrevState，确定返回值
						}
						break;
					}
				}
				else
				{
					//DO并赋值pPrevState
				}
			}
			else
			{
				//
			}
		}
	}
	return nullptr;
}