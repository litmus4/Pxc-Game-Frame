#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CMajorRow
{
public:
	CMajorRow();
	~CMajorRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	int m_iNameTextID;
	int m_iTypeID;
};