#include "InputKeyRow.h"

CInputKeyRow::CInputKeyRow()
{
	//
}

CInputKeyRow::~CInputKeyRow()
{
	//
}

void CInputKeyRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("KeyName", m_strKeyName);
	tabop.GetValue("DisplayName", m_iDisplayTextID);
	tabop.GetValue("MaterialInstancePath", m_strMtlInstFile);
}