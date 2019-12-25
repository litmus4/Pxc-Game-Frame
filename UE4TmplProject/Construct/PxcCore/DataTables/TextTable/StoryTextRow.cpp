#include "StoryTextRow.h"

CStoryTextRow::CStoryTextRow()
{
	//
}

CStoryTextRow::~CStoryTextRow()
{
	//
}

void CStoryTextRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Text", m_strText);
}