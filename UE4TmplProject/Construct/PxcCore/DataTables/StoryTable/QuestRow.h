#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CQuestRow
{
public:
	CQuestRow();
	~CQuestRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
	int m_iDescTextID;
	int m_iContentStoryTextID;

	int m_iGraphID;
	int m_iChapterID;
	int m_iPrevQuestID;
	std::vector<int> m_vecSubPrevQuestIDs;
	bool m_bMainHeadMark;
	int m_iTributaryID;//保证与存档对应上
	bool m_bPrevOR;
	bool m_bNextSingle;

	//接受条件们
	//是否自动接受
	//接受分镜们
	//提交条件们
	//是否自动提交
	//提交分镜们
	//任务奖励
};