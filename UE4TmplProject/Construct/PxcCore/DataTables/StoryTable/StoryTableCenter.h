#pragma once
#include "PxcUtil/Singleton.h"
#include "QuestRow.h"

class CStoryTableCenter : public PxcUtil::Singleton<CStoryTableCenter>
{
DECLARE_SINGLETON_CONSTRUCTOR(CStoryTableCenter)
public:
	~CStoryTableCenter();
	bool Init(const std::string& strPath);
	void Release();

	CQuestRow* GetQuestRow(int iID);
	void GetNextQuestRows(int iID, std::vector<CQuestRow*>& vecOut);

private:
	void AddNextQuest(int iID, int iNextID);

	std::map<int, CQuestRow*> m_mapQuests;
	std::map<int, std::vector<int>> m_mapNextQuests;
};