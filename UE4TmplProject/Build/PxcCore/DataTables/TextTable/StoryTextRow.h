#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CStoryTextRow
{
public:
	CStoryTextRow();
	~CStoryTextRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	std::string m_strText;
};