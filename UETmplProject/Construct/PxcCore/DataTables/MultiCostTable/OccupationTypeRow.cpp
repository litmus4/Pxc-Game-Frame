#include "OccupationTypeRow.h"

COccupationTypeRow::COccupationTypeRow()
{
	//
}

COccupationTypeRow::~COccupationTypeRow()
{
	//
}

void COccupationTypeRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_iNameTextID);
	tabop.GetValue("TypeType", m_strTypeType);
}