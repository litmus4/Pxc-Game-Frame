#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CCaptionRow
{
public:
	CCaptionRow();
	~CCaptionRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	std::string m_strText;
	std::string m_strTag;
};