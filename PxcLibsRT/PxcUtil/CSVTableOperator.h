#pragma once
#include "CSVOperator.h"
//#include <sstream>

namespace PxcUtil
{

class CCSVTableOperator
{
public:
	enum ECol_Type
	{
		ECol_Int = 0,
		ECol_Float,
		ECol_String,
		ECol_IntArray,
		ECol_FloatArray,
		ECol_StringArray,
	};
	enum ERowForColHead
	{
		ERowForColHead_Desc = 1,
		ERowForColHead_Name,
		ERowForColHead_Type,
		ERowForColHead_Default,
	};

	struct ColHead
	{
		u32 uKey;
		ECol_Type eType;
		std::string strCurValue;
	};

public:
	CCSVTableOperator();
	~CCSVTableOperator();

	bool Load(const char* szFile);
	bool Save(const char* szFile = NULL);
	void Reset();
	void ResetIter();

	bool ReadRow();
	bool AddColumn(const std::string& strName, ECol_Type eType, const std::string& strDefault, const std::string& strDesc = "");
	bool WriteRow();
	ECol_Type GetType(const std::string& strColName);

	template<typename T>
	bool GetValue(const std::string& strColName, T& outValue)
	{
		if (m_iRowNum < 0)
			return false;
		std::map<std::string, ColHead>::iterator iter = m_mapColHeads.find(strColName);
		if (iter != m_mapColHeads.end())
		{
			std::stringstream stream;
			stream << iter->second.strCurValue;
			stream >> outValue;
			if (iter->second.strCurValue.empty())
				return true;
			else
				return !(stream.fail()||stream.bad());
		}
		return false;
	}

	template<typename T>
	bool GetArray(const std::string& strColName, std::vector<T>& vecOut)
	{
		if (m_iRowNum < 0)
			return false;
		std::map<std::string, ColHead>::iterator iter = m_mapColHeads.find(strColName);
		if (iter != m_mapColHeads.end() && iter->second.eType >= ECol_IntArray)
		{
			std::vector<std::vector<T>> vecMid;
			if (StringParser::GetParamFromArea(iter->second.strCurValue, vecMid) > 0)
				vecOut = vecMid[0];
			return true;
		}
		return false;
	}

	template<typename T>
	bool SetValue(const std::string& strColName, const T& value)
	{
		if (m_iRowNum < 0)
			return false;
		std::map<std::string, ColHead>::iterator iter = m_mapColHeads.find(strColName);
		if (iter != m_mapColHeads.end())
		{
			std::stringstream stream;
			stream << value;
			iter->second.strCurValue = stream.str().c_str();
			return true;
		}
		return false;
	}

	template<typename T>
	bool SetArray(const std::string& strColName, std::vector<T>& vecValues)
	{
		if (m_iRowNum < 0)
			return false;
		std::map<std::string, ColHead>::iterator iter = m_mapColHeads.find(strColName);
		if (iter != m_mapColHeads.end() && iter->second.eType >= ECol_IntArray)
		{
			iter->second.strCurValue = StringParser::SetParamToSimpleArea(vecValues);
			return true;
		}
		return false;
	}

	static ECol_Type ColTypeStringToEnum(const std::string& str);
	static std::string ColTypeEnumToString(ECol_Type eType);

private:
	CCSVOperator m_Core;
	std::map<std::string, ColHead> m_mapColHeads;
	int m_iRowNum;//-1ÎªÎÞÐ§
	std::map<u32, std::map<u32, std::string>>::iterator m_itRow;
	std::map<u32, std::string>* m_pmapDefaults;
};

}