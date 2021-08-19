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
			std::map<int, std::vector<int>>::iterator itPrev =
				m_mapPrevQuests.find(iter->second->m_iPrevQuestID);
			if (itPrev == m_mapPrevQuests.end())
			{
				std::vector<int> vecIDs;
				vecIDs.push_back(iter->first);
				m_mapPrevQuests.insert(std::make_pair(iter->second->m_iPrevQuestID, vecIDs));
			}
			else
				itPrev->second.push_back(iter->first);
		}
	}
}

void CStoryTableCenter::Release()
{
	UNLOADTABLE(Quest, m_mapQuests)
	m_mapPrevQuests.clear();

	DeleteInstance();
}

CQuestRow* CStoryTableCenter::GetQuestRow(int iID)
{
	std::map<int, CQuestRow*>::iterator iter = m_mapQuests.find(iID);
	if (iter != m_mapQuests.end())
		return iter->second;
	return NULL;
}

void CStoryTableCenter::GetQuestRowsByPrev(int iID, std::vector<CQuestRow*>& vecOut)
{
	std::map<int, std::vector<int>>::iterator itPrev = m_mapPrevQuests.find(iID);
	if (itPrev != m_mapPrevQuests.end())
	{
		std::vector<int>::iterator itSub = itPrev->second.begin();
		for (; itSub != itPrev->second.end(); itSub++)
		{
			std::map<int, CQuestRow*>::iterator iter = m_mapQuests.find(*itSub);
			if (iter != m_mapQuests.end())
				vecOut.push_back(iter->second);
		}
	}
}