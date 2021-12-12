#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CInputActionNameRow
{
public:
	CInputActionNameRow();
	~CInputActionNameRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	std::string m_strActionName;
	int m_iDisplayTextID;
	bool m_bAxis;
	bool m_bAxisPositive;
	bool m_bGamepadModifyForbidden;
	bool m_bKeyboardModifyForbidden;
};