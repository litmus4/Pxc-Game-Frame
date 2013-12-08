#include "CSVTableOperator.h"

#define STR_EMPTY "[empty]"

namespace PxcUtil
{

CCSVTableOperator::CCSVTableOperator()
{
	m_iRowNum = -1;
	m_pmapDefaults = NULL;
}

CCSVTableOperator::~CCSVTableOperator()
{
	//
}

bool CCSVTableOperator::Load(const char* szFile)
{
	if (!m_Core.LoadCSV(szFile))
		return false;

	if (m_Core.GetCSVMap().size() >= ERowForColHead_Default)
	{
		std::map<u32, std::string>& mapNames = m_Core.GetCSVMap()[ERowForColHead_Name];
		std::map<u32, std::string>& mapTypes = m_Core.GetCSVMap()[ERowForColHead_Type];
		std::map<u32, std::string>& mapDefaults = m_Core.GetCSVMap()[ERowForColHead_Default];
		if (mapNames.size() <= mapTypes.size() && mapNames.size() <= mapDefaults.size())
		{
			std::map<u32, std::string>::iterator iter = mapNames.begin();
			for (; iter != mapNames.end(); iter++)
			{
				ColHead head;
				head.uKey = iter->first;
				head.eType = ColTypeStringToEnum(mapTypes[iter->first]);
				m_mapColHeads.insert(std::make_pair(iter->second, head));
			}

			m_iRowNum = m_Core.GetCSVMap().size() - ERowForColHead_Default;
			m_itRow = m_Core.GetCSVMap().find(ERowForColHead_Default + 1);
			m_pmapDefaults = &mapDefaults;
			return true;
		}
	}

	Reset();
	return false;
}

bool CCSVTableOperator::Save(const char* szFile)
{
	//
	return false;
}

void CCSVTableOperator::Reset()
{
	m_Core.GetCSVMap().clear();
	m_mapColHeads.clear();
	m_iRowNum = -1;
	m_pmapDefaults = NULL;
}

void CCSVTableOperator::ResetIter()
{
	if (m_iRowNum < 0)
		return;
	m_itRow = m_Core.GetCSVMap().find(ERowForColHead_Default + 1);
}

bool CCSVTableOperator::ReadRow()
{
	if (m_iRowNum < 0 || m_pmapDefaults == NULL)
		return false;

	if (m_itRow != m_Core.GetCSVMap().end())
	{
		std::map<std::string, ColHead>::iterator iter = m_mapColHeads.begin();
		for (; iter != m_mapColHeads.end(); iter++)
		{
			std::string* pstrValue = NULL;
			std::map<u32, std::string>::iterator itCol = m_itRow->second.find(iter->second.uKey);
			if (itCol != m_itRow->second.end())
				pstrValue = &itCol->second;
			else
				pstrValue = &(*m_pmapDefaults)[iter->second.uKey];

			if (iter->second.eType == ECol_String && *pstrValue == STR_EMPTY)
				iter->second.strCurValue.clear();
			else
				iter->second.strCurValue = *pstrValue;
		}
		m_itRow++;
		return true;
	}
	else
		return false;
}

CCSVTableOperator::ECol_Type CCSVTableOperator::GetType(const std::string& strColName)
{
	if (m_iRowNum < 0)
		return ECol_String;

	std::map<std::string, ColHead>::iterator iter = m_mapColHeads.find(strColName);
	if (iter != m_mapColHeads.end())
		return iter->second.eType;
	else
		return ECol_String;
}

CCSVTableOperator::ECol_Type CCSVTableOperator::ColTypeStringToEnum(const std::string& str)
{
	if (str == "int") return ECol_Int;
	else if (str == "float") return ECol_Float;
	else if (str == "string") return ECol_String;
	else if (str.size() > 7)
	{
		if (str.substr(0, 6) == "array[")
		{
			std::string strSub = str.substr(6, str.size() - 7);
			if (strSub == "int") return ECol_IntArray;
			else if (strSub == "float") return ECol_FloatArray;
			else if (strSub == "string") return ECol_StringArray;
		}
	}
	return ECol_String;
}

}