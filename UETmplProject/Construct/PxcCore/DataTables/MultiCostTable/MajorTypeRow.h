#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CMajorTypeRow
{
public:
	CMajorTypeRow();
	~CMajorTypeRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
};