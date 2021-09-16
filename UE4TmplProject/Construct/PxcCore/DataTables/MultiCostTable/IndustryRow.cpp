#include "IndustryRow.h"

CIndustryRow::CIndustryRow()
{
	//
}

CIndustryRow::~CIndustryRow()
{
	//
}

void CIndustryRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_iNameTextID);
	tabop.GetValue("Type", m_iTypeID);
	tabop.GetArray("IdleOccupations", m_vecOccupations);
	tabop.GetArray("OccupationTypes", m_vecOccupationTypes);
}