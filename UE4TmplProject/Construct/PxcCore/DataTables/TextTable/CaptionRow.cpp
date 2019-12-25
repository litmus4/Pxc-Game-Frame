#include "CaptionRow.h"

CCaptionRow::CCaptionRow()
{
	//
}

CCaptionRow::~CCaptionRow()
{
	//
}

void CCaptionRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Text", m_strText);
	tabop.GetValue("Tag", m_strTag);
}