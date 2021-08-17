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
	tabop.GetValue("GroupID", m_iGraphID);
	tabop.GetValue("ChapterID", m_iChapterID);
	//
}