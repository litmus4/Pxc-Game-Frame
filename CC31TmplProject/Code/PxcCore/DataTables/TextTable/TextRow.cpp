#include "TextRow.h"

CTextRow::CTextRow()
{
	//
}

CTextRow::~CTextRow()
{
	//
}

void CTextRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Text", m_strText);
}