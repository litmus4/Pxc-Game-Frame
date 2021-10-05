#include "QuestRow.h"

CQuestRow::CQuestRow()
{
	//
}

CQuestRow::CQuestRow(CQuestRow* pOtherRow)
{
	if (!pOtherRow) return;
	m_iID = pOtherRow->m_iID;
	m_iNameTextID = -1;
	m_iDescTextID = -1;
	m_iContentStoryTextID = -1;

	m_iGraphID = pOtherRow->m_iGraphID;
	m_iChapterID = pOtherRow->m_iChapterID;
	m_iPrevQuestID = -1;
	m_iHeadLevel = pOtherRow->m_iHeadLevel;
	m_iParentHeadQuestID = pOtherRow->m_iParentHeadQuestID;
	m_bPrevOR = false;
	m_ePrevConfType = StoryDef::EPrevConfluentType::EPrevNormal;
	m_bNextSingle = false;
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
	tabop.GetValue("IsPrevOR", m_bPrevOR);
	tabop.GetValue("PrevConfluentType", *(int*)&m_ePrevConfType);
	tabop.GetValue("IsNextSingle", m_bNextSingle);
}