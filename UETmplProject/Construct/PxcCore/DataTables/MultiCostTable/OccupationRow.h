#pragma once
#include "PxcUtil/CSVTableOperator.h"

class COccupationRow
{
public:
	COccupationRow();
	~COccupationRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
	int m_iTypeID;
};