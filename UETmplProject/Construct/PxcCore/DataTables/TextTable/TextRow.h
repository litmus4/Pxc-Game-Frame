#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CTextRow
{
public:
	CTextRow();
	~CTextRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	std::string m_strText;
};