#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CIndustryRow
{
public:
	CIndustryRow();
	~CIndustryRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
	int m_iTypeID;
	std::vector<int> m_vecOccupations;
	std::vector<int> m_vecOccupationTypes;
};