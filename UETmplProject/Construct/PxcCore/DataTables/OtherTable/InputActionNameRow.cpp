#include "InputActionNameRow.h"

CInputActionNameRow::CInputActionNameRow()
{
	//
}

CInputActionNameRow::~CInputActionNameRow()
{
	//
}

void CInputActionNameRow::Read(PxcUtil::CCSVTableOperator& tabop)
{
	tabop.GetValue("ID", m_iID);
	tabop.GetValue("ActionName", m_strActionName);
	tabop.GetValue("DisplayText", m_iDisplayTextID);

	int iBool = 0;
	tabop.GetValue("IsAxis", iBool);
	m_bAxis = (bool)iBool;
	tabop.GetValue("IsAxisPositive", iBool);
	m_bAxisPositive = (bool)iBool;
	tabop.GetValue("IsGamepadModifyForbidden", iBool);
	m_bGamepadModifyForbidden = (bool)iBool;
	tabop.GetValue("IsKeyboardModifyForbidden", iBool);
	m_bKeyboardModifyForbidden = (bool)iBool;
}