// Fill out your copyright notice in the Description page of Project Settings.


#include "Story/OsQuestMgr.h"
#include "Framework/PxcGameConfig.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/StoryTable/StoryTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

bool UOsQuestMgr::SQuestFsmEx::operator==(const int32& iOtherID) const
{
	return (iHeadStateID == iOtherID);
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

			CQuestRow* pRow = *iter;
			std::map<int32, SSavedHead>::iterator itSaved = m_mapSavedHeads.find((*iter)->m_iGraphID);
			if (itSaved != m_mapSavedHeads.end())
			{
				pRow = CStoryTableCenter::GetInstance()->GetQuestRow(itSaved->second.iID);
				check(pRow);
				pState->Init(pRow);
				itSaved->second.bInit = true;
			}

			SQuestFsmEx FsmEx;
			FsmEx.iCurStateID = pRow->m_iID;
			FsmEx.iHeadStateID = (*iter)->m_iID;
			FsmEx.Fsm.AddState(pState);
			FsmEx.Fsm.SetState(pState, true);
			std::pair<std::map<int32, SQuestFsmEx>::iterator, bool> RetPair =
				m_mapQuestFsms.insert(std::make_pair((*iter)->m_iGraphID, FsmEx));

			std::map<int32, std::vector<UQuestState*>>::iterator itGh = mapGraphHeads.find((*iter)->m_iGraphID);
			if (itGh != mapGraphHeads.end())
			{
				std::vector<UQuestState*>::iterator itHead = itGh->second.begin();
				for (; itHead != itGh->second.end(); itHead++)
				{
					AddTributaryHead(RetPair.first->second.vecTributaries, *itHead, (*itHead)->GetQuestRow(),
						mapTribuHeads, (itSaved != m_mapSavedHeads.end() ? &itSaved->second.mapSavedTribuHeads : nullptr), 1);
				}
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
			{
				std::map<int32, SSavedHead>::iterator itSaved = m_mapSavedHeads.find((*iter)->m_iGraphID);
				AddTributaryHead(itQf->second.vecTributaries, pState, *iter,
					mapTribuHeads, (itSaved != m_mapSavedHeads.end() ? &itSaved->second.mapSavedTribuHeads : nullptr), 1);
			}
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
			std::map<int32, SSavedHead>::iterator itSaved = m_mapSavedHeads.find(itGh->first);
			if (bMain)
			{
				SQuestFsmEx FsmEx;

				FsmEx.iHeadStateID = (*itHead)->GetQuestID();
				if (itSaved != m_mapSavedHeads.end())
				{
					CQuestRow* pRow = CStoryTableCenter::GetInstance()->GetQuestRow(itSaved->second.iID);
					verify(pRow);
					(*itHead)->Init(pRow);
					itSaved->second.bInit = true;
				}

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

					TriFsmEx.iHeadStateID = (*itHead)->GetQuestID();
					if (itSaved != m_mapSavedHeads.end())
					{
						std::map<int32, SSavedHead>::iterator itSavedTri =
							itSaved->second.mapSavedTribuHeads.find((*itHead)->GetQuestID());
						if (itSavedTri != itSaved->second.mapSavedTribuHeads.end())
						{
							CQuestRow* pRow = CStoryTableCenter::GetInstance()->GetQuestRow(itSavedTri->second.iID);
							verify(pRow);
							(*itHead)->Init(pRow);
							itSavedTri->second.bInit = true;
						}
					}

					TriFsmEx.iCurStateID = (*itHead)->GetQuestID();
					TriFsmEx.Fsm.AddState(*itHead);
					TriFsmEx.Fsm.SetState(*itHead, true);
					itQf->second.vecTributaries.push_back(TriFsmEx);
				}
			}
		}
	}

	//�ݹ鴦�����δInit�Ĵ浵��AND��������ʱ�������֧����
	std::map<int32, SSavedHead>::iterator itSaved = m_mapSavedHeads.begin();
	for (; itSaved != m_mapSavedHeads.end(); itSaved++)
	{
		if (itSaved->second.bInit)
			continue;

		verify(m_mapQuestFsms.find(itSaved->first) == m_mapQuestFsms.end());
		CQuestRow* pRow = CStoryTableCenter::GetInstance()->GetQuestRow(itSaved->second.iID);
		check(pRow);
		UQuestState* pState = NewObject<UQuestState>();
		pState->Init(pRow);
		m_tmapQuestStates.Add(pRow->m_iID, pState);

		SQuestFsmEx FsmEx;
		FsmEx.iCurStateID = pRow->m_iID;
		FsmEx.iHeadStateID = itSaved->second.iHeadID;
		FsmEx.Fsm.AddState(pState);
		FsmEx.Fsm.SetState(pState, true);
		std::pair<std::map<int32, SQuestFsmEx>::iterator, bool> RetPair =
			m_mapQuestFsms.insert(std::make_pair(pRow->m_iGraphID, FsmEx));

		SupplyExtraTributaryHeads(RetPair.first->second.vecTributaries, itSaved->second.mapSavedTribuHeads);
		itSaved->second.bInit = true;
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
			bool bFindMain = false;
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
						pPrevState = pState;
					}
					else
						verify(false);//���ɵĽ��㴦������SubPrev
				}
				else
				{
					if (itQf->second.Fsm.GetStateNum() >= GetDefault<UPxcGameConfig>()->iQuestFsmSubCutStateNum)
					{
						vecNextRows.clear();
						break;
					}

					ExtendSubNext(itQf->second, pPrevState, *itNext, itNsp, 1, 0, &pConfState);
					UQuestState** ppHeadState = m_tmapQuestStates.Find((*itNext)->m_iID);
					if (ppHeadState)
					{
						if (!bFindMain && pConfState)
						{
							(*ppHeadState)->m_iRuntimeHeadLevel = 0;
							bFindMain = true;
						}
						else
							(*ppHeadState)->m_iRuntimeHeadLevel = 1;
					}
				}
			}

			if (itQf->second.Fsm.GetStateNum() < GetDefault<UPxcGameConfig>()->iQuestFsmMaxStateNum)
			{
				if (vecNextRows.size() > 1)
				{
					//FLAGJK �������ȷ��pConfState(����)
					pPrevState = pConfState;
				}
				else if (vecNextRows.empty())
					pPrevState = nullptr;
			}
			else
				pPrevState = nullptr;
		}

		ExtendTributaries(itQf->second.vecTributaries, 1);
	}
}

void UOsQuestMgr::Release()
{
	m_mapQuestFsms.clear();
	m_tmapQuestStates.Empty();
}

void UOsQuestMgr::AddTributaryHead(std::vector<SQuestFsmEx>& vecTributaries, UQuestState* pState, CQuestRow* pRow,
	std::unordered_map<int32, std::vector<UQuestState*>>& mapTribuHeads,
	std::map<int32, SSavedHead>* pmapSavedTribuHeads, int32 iLevel)
{
	if (pRow->m_iHeadLevel == iLevel)
	{
		SQuestFsmEx FsmEx;

		FsmEx.iHeadStateID = pRow->m_iID;
		std::map<int32, SSavedHead>* pmapSubSavedTriHeads = nullptr;
		if (pmapSavedTribuHeads)
		{
			std::map<int32, SSavedHead>::iterator itSaved = pmapSavedTribuHeads->find(pRow->m_iID);
			if (itSaved != pmapSavedTribuHeads->end())
			{
				pRow = CStoryTableCenter::GetInstance()->GetQuestRow(itSaved->second.iID);
				check(pRow);
				pState->Init(pRow);
				itSaved->second.bInit = true;
				pmapSubSavedTriHeads = &itSaved->second.mapSavedTribuHeads;
			}
		}

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
			{
				AddTributaryHead(FsmPushed.vecTributaries, *itHead, (*itHead)->GetQuestRow(),
					mapTribuHeads, pmapSubSavedTriHeads, iLevel + 1);
			}
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
		{
			std::map<int32, SSavedHead>* pmapSubSavedTriHeads = nullptr;
			if (pmapSavedTribuHeads)
			{
				std::map<int32, SSavedHead>::iterator itSaved = pmapSavedTribuHeads->find(pUpRow->m_iID);
				if (itSaved != pmapSavedTribuHeads->end())
					pmapSubSavedTriHeads = &itSaved->second.mapSavedTribuHeads;
			}
			AddTributaryHead(iter->vecTributaries, pState, pRow, mapTribuHeads, pmapSubSavedTriHeads, iLevel + 1);
		}
	}
}

void UOsQuestMgr::SupplyExtraTributaryHeads(std::vector<SQuestFsmEx>& vecTributaries,
	std::map<int32, SSavedHead>& mapSavedTribuHeads)
{
	std::map<int32, SSavedHead>::iterator itSaved = mapSavedTribuHeads.begin();
	for (; itSaved != mapSavedTribuHeads.end(); itSaved++)
	{
		if (itSaved->second.bInit)
			continue;

		CQuestRow* pRow = CStoryTableCenter::GetInstance()->GetQuestRow(itSaved->second.iID);
		check(pRow);
		UQuestState* pState = NewObject<UQuestState>();
		pState->Init(pRow);
		m_tmapQuestStates.Add(pRow->m_iID, pState);

		SQuestFsmEx FsmEx;
		FsmEx.iCurStateID = pRow->m_iID;
		FsmEx.iHeadStateID = itSaved->second.iHeadID;
		FsmEx.Fsm.AddState(pState);
		FsmEx.Fsm.SetState(pState, true);
		vecTributaries.push_back(FsmEx);
		SQuestFsmEx& FsmPushed = vecTributaries[vecTributaries.size() - 1];

		SupplyExtraTributaryHeads(FsmPushed.vecTributaries, itSaved->second.mapSavedTribuHeads);
		itSaved->second.bInit = true;
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
		bool bFindMain = false;
		std::vector<CQuestRow*>::iterator itNext = vecNextRows.begin();
		for (; itNext != vecNextRows.end(); itNext++)
		{
			std::vector<int>::iterator itNsp = std::find((*itNext)->m_vecSubPrevQuestIDs.begin(),//Next Sub Prev
				(*itNext)->m_vecSubPrevQuestIDs.end(), pPrevState->GetQuestID());
			if (vecNextRows.size() == 1)
				LinkOnce(*itNext, itNsp, &pConfState);
			else
			{
				ExtendSubNext(FsmEx, pPrevState, *itNext, itNsp, iLevel + 1, iTriLevel, &pConfState);
				UQuestState** ppHeadState = m_tmapQuestStates.Find((*itNext)->m_iID);
				if (ppHeadState)
				{
					if (!bFindMain && pConfState)
					{
						(*ppHeadState)->m_iRuntimeHeadLevel = iLevel;
						bFindMain = true;
					}
					else
						(*ppHeadState)->m_iRuntimeHeadLevel = iLevel + 1;
				}
			}
		}

		if (vecNextRows.size() > 1)
		{
			//FLAGJK �������ȷ��pConfState(����)
			pPrevState = pConfState;
		}
		else if (vecNextRows.empty())
			pPrevState = nullptr;
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

void UOsQuestMgr::ExtendTributaries(std::vector<SQuestFsmEx>& vecTributaries, int32 iLevel)
{
	std::vector<SQuestFsmEx>::iterator iter = vecTributaries.begin();
	for (; iter != vecTributaries.end(); iter++)
	{
		UQuestState* pPrevState = iter->Fsm.GetCurrentState();
		while (pPrevState)
		{
			std::vector<CQuestRow*> vecNextRows;
			CStoryTableCenter::GetInstance()->GetNextQuestRows(pPrevState->GetQuestID(), vecNextRows);
			UQuestState* pConfState = nullptr;
			bool bFindMain = false;
			std::vector<CQuestRow*>::iterator itNext = vecNextRows.begin();
			for (; itNext != vecNextRows.end(); itNext++)
			{
				std::vector<int>::iterator itNsp = std::find((*itNext)->m_vecSubPrevQuestIDs.begin(),//Next Sub Prev
					(*itNext)->m_vecSubPrevQuestIDs.end(), pPrevState->GetQuestID());
				if (vecNextRows.size() == 1)
				{
					if (itNsp == (*itNext)->m_vecSubPrevQuestIDs.end())
					{
						UQuestState* pState = NewObject<UQuestState>();
						pState->Init(*itNext);
						m_tmapQuestStates.Add((*itNext)->m_iID, pState);

						iter->Fsm.AddState(pState);
						iter->Fsm.AddTransfer(pPrevState, (*itNext)->m_iID, pState);
						pPrevState = pState;
					}
				}
				else
				{
					if (iter->Fsm.GetStateNum() >= GetDefault<UPxcGameConfig>()->iQuestFsmSubCutStateNum)
					{
						vecNextRows.clear();
						break;
					}

					ExtendSubNext(*iter, pPrevState, *itNext, itNsp, 1, iLevel, &pConfState);
					UQuestState** ppHeadState = m_tmapQuestStates.Find((*itNext)->m_iID);
					if (ppHeadState)
					{
						if (!bFindMain && pConfState)
						{
							(*ppHeadState)->m_iRuntimeHeadLevel = iLevel;
							bFindMain = true;
						}
						else
							(*ppHeadState)->m_iRuntimeHeadLevel = iLevel + 1;
					}
				}
			}

			if (iter->Fsm.GetStateNum() < GetDefault<UPxcGameConfig>()->iQuestFsmMaxStateNum)
			{
				if (vecNextRows.size() > 1)
				{
					//FLAGJK �������ȷ��pConfState(����)
					pPrevState = pConfState;
				}
				else if (vecNextRows.empty())
					pPrevState = nullptr;
			}
			else
				pPrevState = nullptr;
		}

		ExtendTributaries(iter->vecTributaries, iLevel + 1);
	}
}