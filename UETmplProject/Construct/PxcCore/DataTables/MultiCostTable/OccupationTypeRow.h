#pragma once
#include "PxcUtil/CSVTableOperator.h"

class COccupationTypeRow
{
public:
	COccupationTypeRow();
	~COccupationTypeRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
	std::string m_strTypeType;
};