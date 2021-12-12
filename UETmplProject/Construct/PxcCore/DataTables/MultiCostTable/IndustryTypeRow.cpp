#include "IndustryTypeRow.h"

CIndustryTypeRow::CIndustryTypeRow()
{
	//
}

CIndustryTypeRow::~CIndustryTypeRow()
{
	//
}

void CIndustryTypeRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_iNameTextID);
}