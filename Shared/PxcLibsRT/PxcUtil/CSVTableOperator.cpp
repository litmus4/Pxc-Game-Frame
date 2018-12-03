#include "CSVTableOperator.h"

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
				head.bGet = false;
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
	if (szFile == NULL && m_Core.IsNoName())
		return false;
	return m_Core.SaveCSV(szFile);
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

	if (m_itRow != m_Core.GetCSVMap().end() && m_itRow->second.size() > 1)//·ÀÂÒÂë
	{
		std::map<std::string, ColHead>::iterator iter = m_mapColHeads.begin();
		for (; iter != m_mapColHeads.end(); iter++)
		{
			std::map<u32, std::string>::iterator itCol = m_itRow->second.find(iter->second.uKey);
			if (itCol != m_itRow->second.end() && !itCol->second.empty())
				iter->second.strCurValue = itCol->second;
			else
				iter->second.strCurValue = (*m_pmapDefaults)[iter->second.uKey];
			iter->second.bGet = false;
		}
		m_itRow++;
		return true;
	}
	else
		return false;
}

bool CCSVTableOperator::AddColumn(const std::string& strName, ECol_Type eType, const std::string& strDefault, const std::string& strDesc)
{
	if (m_mapColHeads.find(strName) != m_mapColHeads.end())
		return false;
	ColHead head;
	head.eType = eType;
	head.bGet = false;

	std::map<u32, std::map<u32, std::string>>::iterator itRowName = m_Core.GetCSVMap().find(ERowForColHead_Name);
	if (itRowName != m_Core.GetCSVMap().end())
	{
		std::map<u32, std::string>& mapDescs = m_Core.GetCSVMap()[ERowForColHead_Desc];
		std::map<u32, std::string>& mapNames = itRowName->second;
		std::map<u32, std::string>& mapTypes = m_Core.GetCSVMap()[ERowForColHead_Type];
		if (m_pmapDefaults == NULL)
			m_pmapDefaults = &(m_Core.GetCSVMap()[ERowForColHead_Default]);

		std::map<u32, std::string>::iterator itColNameMax = mapNames.end();
		itColNameMax--;
		u32 uNewKey = itColNameMax->first + 1;
		mapDescs.insert(std::make_pair(uNewKey, strDesc));
		mapNames.insert(std::make_pair(uNewKey, strName));
		mapTypes.insert(std::make_pair(uNewKey, ColTypeEnumToString(eType)));
		m_pmapDefaults->insert(std::make_pair(uNewKey, strDefault));

		head.uKey = uNewKey;
	}
	else
	{
		std::map<u32, std::string> mapDescs;
		mapDescs.insert(std::make_pair(1, strDesc));
		m_Core.GetCSVMap().insert(std::make_pair(ERowForColHead_Desc, mapDescs));

		std::map<u32, std::string> mapNames;
		mapNames.insert(std::make_pair(1, strName));
		m_Core.GetCSVMap().insert(std::make_pair(ERowForColHead_Name, mapNames));

		std::map<u32, std::string> mapTypes;
		mapTypes.insert(std::make_pair(1, ColTypeEnumToString(eType)));
		m_Core.GetCSVMap().insert(std::make_pair(ERowForColHead_Type, mapTypes));

		std::map<u32, std::string> mapDefaults;
		mapDefaults.insert(std::make_pair(1, strDefault));
		std::pair<std::map<u32, std::map<u32, std::string>>::iterator, bool> pairRet =
			m_Core.GetCSVMap().insert(std::make_pair(ERowForColHead_Default, mapDefaults));

		m_pmapDefaults = &pairRet.first->second;
		head.uKey = 1;
	}

	m_mapColHeads.insert(std::make_pair(strName, head));
	if (m_iRowNum < 0)
	{
		m_iRowNum = 0;
		m_itRow = m_Core.GetCSVMap().end();
	}
	return true;
}

bool CCSVTableOperator::WriteRow()
{
	if (m_iRowNum < 0)
		return false;

	std::map<u32, std::string> mapRow;
	std::map<std::string, ColHead>::iterator iter = m_mapColHeads.begin();
	for (; iter != m_mapColHeads.end(); iter++)
		mapRow.insert(std::make_pair(iter->second.uKey, iter->second.strCurValue));
	std::map<u32, std::map<u32, std::string>>::iterator itRowMax = m_Core.GetCSVMap().end();
	itRowMax--;
	m_Core.GetCSVMap().insert(std::make_pair(itRowMax->first + 1, mapRow));
	return true;
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

void CCSVTableOperator::ColIter::Next()
{
	do 
	{
		iter++;
	} while (Ok() && iter->second.bGet);
}

bool CCSVTableOperator::ColIter::Ok()
{
	return (iter != itEnd);
}

std::string CCSVTableOperator::ColIter::GetName()
{
	return iter->first;
}

CCSVTableOperator::ECol_Type CCSVTableOperator::ColIter::GetType()
{
	return iter->second.eType;
}

CCSVTableOperator::ColIter CCSVTableOperator::Begin()
{
	ColIter colit;
	colit.iter = colit.itEnd = m_mapColHeads.end();
	if (m_iRowNum >= 0)
	{
		colit.iter = m_mapColHeads.begin();
		while (colit.Ok() && colit.iter->second.bGet)
			colit.iter++;
	}
	return colit;
}

CCSVTableOperator::ECol_Type CCSVTableOperator::ColTypeStringToEnum(const std::string& str)
{
	if (str == "int") return ECol_Int;
	else if (str == "int64") return ECol_Int64;
	else if (str == "float") return ECol_Float;
	else if (str == "string") return ECol_String;
	else if (str.size() > 7)
	{
		if (str.substr(0, 6) == "array[")
		{
			std::string strSub = str.substr(6, str.size() - 7);
			if (strSub == "int") return ECol_IntArray;
			else if (strSub == "int64") return ECol_Int64Array;
			else if (strSub == "float") return ECol_FloatArray;
			else if (strSub == "string") return ECol_StringArray;
		}
	}
	return ECol_String;
}

std::string CCSVTableOperator::ColTypeEnumToString(ECol_Type eType)
{
	switch (eType)
	{
	case ECol_Int: return "int";
	case ECol_Int64: return "int64";
	case ECol_Float: return "float";
	case ECol_String: return "string";
	case ECol_IntArray: return "array[int]";
	case ECol_Int64Array: return "array[int64]";
	case ECol_FloatArray: return "array[float]";
	case ECol_StringArray: return "array[string]";
	}
	return "";
}

}