#pragma once
#include "PxcUtil/CSVTableOperator.h"
#include "..\LoadDefines.h"

class CGlobalParamRow
{
public:
	CGlobalParamRow();
	~CGlobalParamRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	std::string m_strName;
	
	DECLARE_LEAVECOL_MEMBERS
};