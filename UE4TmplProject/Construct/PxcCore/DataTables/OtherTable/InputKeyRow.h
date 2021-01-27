#pragma once
#include "PxcUtil/CSVTableOperator.h"

class CInputKeyRow
{
public:
	CInputKeyRow();
	~CInputKeyRow();
	void Read(PxcUtil::CCSVTableOperator& tabop);

	int m_iID;
	std::string m_strKeyName;
	std::string m_strDisplayName;
	std::string m_strMtlInstFile;
};