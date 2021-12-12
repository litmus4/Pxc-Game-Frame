#include "OccupationRow.h"

COccupationRow::COccupationRow()
{
	//
}

COccupationRow::~COccupationRow()
{
	//
}

void COccupationRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_iNameTextID);
	tabop.GetValue("Type", m_iTypeID);
}