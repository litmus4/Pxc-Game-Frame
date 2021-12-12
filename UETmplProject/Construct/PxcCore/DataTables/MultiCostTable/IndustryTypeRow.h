#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CIndustryTypeRow
{
public:
	CIndustryTypeRow();
	~CIndustryTypeRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
};