#include "QuestRow.h"

CQuestRow::CQuestRow()
{
	//
}

CQuestRow::~CQuestRow()
{
	//
}

void CQuestRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_iNameTextID);
	tabop.GetValue("Desc", m_iDescTextID);
	tabop.GetValue("Content", m_iContentStoryTextID);

	tabop.GetValue("Group", m_iGraphID);
	tabop.GetValue("Chapter", m_iChapterID);
	tabop.GetValue("PrevQuest", m_iPrevQuestID);
	tabop.GetArray("SubPrevQuests", m_vecSubPrevQuestIDs);
	tabop.GetValue("HeadLevel", m_iHeadLevel);
	tabop.GetValue("ParentHeadQuest", m_iParentHeadQuestID);
	tabop.GetValue("TributaryID", m_iTributaryID);
	tabop.GetValue("IsPrevOR", m_bPrevOR);
	tabop.GetValue("PrevConfluentType", *(int*)&m_ePrevConfType);
	tabop.GetValue("IsNextSingle", m_bNextSingle);
}