#include "MajorRow.h"

CMajorRow::CMajorRow()
{
	//
}

CMajorRow::~CMajorRow()
{
	//
}

void CMajorRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_iNameTextID);
	tabop.GetValue("Type", m_iTypeID);
}