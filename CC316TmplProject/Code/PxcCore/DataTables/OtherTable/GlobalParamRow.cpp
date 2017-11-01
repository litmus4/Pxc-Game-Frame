#include "GlobalParamRow.h"

CGlobalParamRow::CGlobalParamRow()
{
	//
}

CGlobalParamRow::~CGlobalParamRow()
{
	RELEASE_LEAVECOLS
}

void CGlobalParamRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("Name", m_strName);

	READ_LEAVECOLS(tabop)
}