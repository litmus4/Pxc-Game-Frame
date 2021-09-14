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

	verify(mapTribuHeads.empty());//����֧��������ͷ
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
			UQuestState* pConfState = nullptr;
			std::vector<CQuestRow*>::iterator itNext = vecNextRows.begin();
			for (; itNext != vecNextRows.end(); itNext++)
			{
				std::vector<int>::iterator itNsp = std::find((*itNext)->m_vecSubPrevQuestIDs.begin(),//Next Sub Prev
					(*itNext)->m_vecSubPrevQuestIDs.end(), pPrevState->GetQuestID());
				//���it���¾�if����ߣ�����Ϊ��Next�ݹ�ʱͬ����Ҫ���жϣ������ڡ���\�����������Ҫѡ�����ɣ���֤��ʽͳһ
				if (vecNextRows.size() == 1)
				{
					if (itNsp == (*itNext)->m_vecSubPrevQuestIDs.end())
					{
						UQuestState* pState = NewObject<UQuestState>();
						pState->Init(*itNext);
						m_tmapQuestStates.Add((*itNext)->m_iID, pState);

						itQf->second.Fsm.AddState(pState);
						itQf->second.Fsm.AddTransfer(pPrevState, (*itNext)->m_iID, pState);
					}
					else
						verify(false);//���ɵĽ��㴦������SubPrev
				}
				else
					ExtendSubNext(itQf->second, pPrevState, *itNext, itNsp, 1, 0, &pConfState);
			}

			if (vecNextRows.size() > 1)
			{
				//FLAGJK �������ȷ��pConfState(����)
				pPrevState = pConfState;
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

void UOsQuestMgr::ExtendSubNext(SQuestFsmEx& FsmEx, UQuestState* pPrevState, CQuestRow* pRow,
	std::vector<int>::iterator itFirstNsp, int32 iLevel, int32 iTriLevel, UQuestState** ppConfState)
{
	auto LinkOnce = [this, &FsmEx, &pPrevState, iLevel, iTriLevel]
		(CQuestRow* pRow, std::vector<int>::iterator itNsp, UQuestState** ppConfState)
	{
		//Sub��Ϊ����Ϊ�����
		//Normal: Sub��-�ָɵ�ͷ�ˣ�Sub��-���ɵ�ͷ��
		//FreshLake: Sub��-����֧���ķָɵ�ͷ�ˣ�Sub��-����
		//Combo: Sub��-��֧����֧���ķָɵ�ͷ�� ��������ָɵ�ͷ�ˣ�Sub��-��֧����֧�������ɵ�ͷ�� �����������ɵ�ͷ��
		//SubΪ��������
		if (!pRow->m_vecSubPrevQuestIDs.empty())
		{
			switch (pRow->m_ePrevConfType)
			{
			case StoryDef::EPrevConfluentType::EPrevNormal:
				if (itNsp != pRow->m_vecSubPrevQuestIDs.end())
					//DO���ÿ�pPrevState
					FsmLinkState(FsmEx, &pPrevState, pRow);
				else
					//DO���ÿ�pPrevState��ȷ������ֵ
					*ppConfState = FsmLinkState(FsmEx, &pPrevState, pRow, false, true);
				break;
			case StoryDef::EPrevConfluentType::EPrevFreshLake:
				if (itNsp != pRow->m_vecSubPrevQuestIDs.end())
				{
					if (iTriLevel > 0)
						//DO���ÿ�pPrevState(�ǲ���֧�����������޷�ȷ��)
						FsmLinkState(FsmEx, &pPrevState, pRow);
					else
						verify(false);
				}
				else
					//DO����ֵpPrevState
					FsmLinkState(FsmEx, &pPrevState, pRow, true);
				break;
			case StoryDef::EPrevConfluentType::EPrevCombo:
				if (itNsp != pRow->m_vecSubPrevQuestIDs.end())
				{
					if (iTriLevel > 0)
						//DO���ÿ�pPrevState(�ǲ���֧�����������޷�ȷ��)
						FsmLinkState(FsmEx, &pPrevState, pRow);
					else
						//DO���ÿ�pPrevState
						FsmLinkState(FsmEx, &pPrevState, pRow);
				}
				else
				{
					if (iTriLevel > 0)
						//DO���ÿ�pPrevState��ȷ������ֵ
						*ppConfState = FsmLinkState(FsmEx, &pPrevState, pRow, false, true);
					else
						//DO���ÿ�pPrevState��ȷ������ֵ
						*ppConfState = FsmLinkState(FsmEx, &pPrevState, pRow, false, true);
				}
				break;
			}
		}
		else
			//DO����ֵpPrevState
			FsmLinkState(FsmEx, &pPrevState, pRow, true);
	};

	LinkOnce(pRow, itFirstNsp, ppConfState);

	while (pPrevState)
	{
		std::vector<CQuestRow*> vecNextRows;
		CStoryTableCenter::GetInstance()->GetNextQuestRows(pPrevState->GetQuestID(), vecNextRows);
		UQuestState* pConfState = nullptr;
		std::vector<CQuestRow*>::iterator itNext = vecNextRows.begin();
		for (; itNext != vecNextRows.end(); itNext++)
		{
			std::vector<int>::iterator itNsp = std::find((*itNext)->m_vecSubPrevQuestIDs.begin(),//Next Sub Prev
				(*itNext)->m_vecSubPrevQuestIDs.end(), pPrevState->GetQuestID());
			if (vecNextRows.size() == 1)
				LinkOnce(*itNext, itNsp, &pConfState);
			else
				ExtendSubNext(FsmEx, pPrevState, *itNext, itNsp, iLevel + 1, iTriLevel, &pConfState);
		}

		if (vecNextRows.size() > 1)
		{
			//FLAGJK �������ȷ��pConfState(����)
			pPrevState = pConfState;
		}
	}
}

UQuestState* UOsQuestMgr::FsmLinkState(SQuestFsmEx& FsmEx, UQuestState** ppPrevState, CQuestRow* pRow,
	bool bSetPrev, bool bRetConfluent)
{
	UQuestState* pState = nullptr;
	UQuestState** ppState = m_tmapQuestStates.Find(pRow->m_iID);
	if (!ppState)
	{
		pState = NewObject<UQuestState>();
		pState->Init(pRow);
		m_tmapQuestStates.Add(pRow->m_iID, pState);
		FsmEx.Fsm.AddState(pState);
	}
	else
		pState = *ppState;
	FsmEx.Fsm.AddTransfer(*ppPrevState, pRow->m_iID, pState);

	*ppPrevState = (bSetPrev ? pState : nullptr);
	return (bRetConfluent ? pState : nullptr);
}