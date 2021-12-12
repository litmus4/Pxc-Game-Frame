#include "MajorTypeRow.h"

CMajorTypeRow::CMajorTypeRow()
{
	//
}

CMajorTypeRow::~CMajorTypeRow()
{
	//
}

void CMajorTypeRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_iNameTextID);
}