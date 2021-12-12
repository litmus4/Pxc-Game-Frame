#include "StoryTableCenter.h"
#include "..\LoadDefines.h"

CStoryTableCenter::CStoryTableCenter()
{
	//
}

CStoryTableCenter::~CStoryTableCenter()
{
	//
}

bool CStoryTableCenter::Init(const std::string& strPath)
{
	LOADTABLE(Quest, strPath, "StoryTable", m_mapQuests, m_iID)
	{
		std::map<int, CQuestRow*>::iterator iter = m_mapQuests.begin();
		for (; iter != m_mapQuests.end(); iter++)
		{
			AddNextQuest(iter->second->m_iPrevQuestID, iter->first);

			std::vector<int>::iterator itSub = iter->second->m_vecSubPrevQuestIDs.begin();
			for (; itSub != iter->second->m_vecSubPrevQuestIDs.end(); itSub++)
				AddNextQuest(*itSub, iter->first);
		}
	}
}

void CStoryTableCenter::Release()
{
	UNLOADTABLE(Quest, m_mapQuests)
	m_mapNextQuests.clear();

	DeleteInstance();
}

CQuestRow* CStoryTableCenter::GetQuestRow(int iID)
{
	std::map<int, CQuestRow*>::iterator iter = m_mapQuests.find(iID);
	if (iter != m_mapQuests.end())
		return iter->second;
	return NULL;
}

void CStoryTableCenter::GetNextQuestRows(int iID, std::vector<CQuestRow*>& vecOut)
{
	std::map<int, std::vector<int>>::iterator itNext = m_mapNextQuests.find(iID);
	if (itNext != m_mapNextQuests.end())
	{
		std::vector<int>::iterator itSub = itNext->second.begin();
		for (; itSub != itNext->second.end(); itSub++)
		{
			std::map<int, CQuestRow*>::iterator iter = m_mapQuests.find(*itSub);
			if (iter != m_mapQuests.end())
				vecOut.push_back(iter->second);
		}
	}
}

void CStoryTableCenter::AddNextQuest(int iID, int iNextID)
{
	std::map<int, std::vector<int>>::iterator itNext = m_mapNextQuests.find(iID);
	if (itNext == m_mapNextQuests.end())
	{
		std::vector<int> vecIDs;
		vecIDs.push_back(iNextID);
		m_mapNextQuests.insert(std::make_pair(iID, vecIDs));
	}
	else
		itNext->second.push_back(iNextID);
}