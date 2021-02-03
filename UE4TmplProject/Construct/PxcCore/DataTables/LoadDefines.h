#pragma once
#include "PxcUtil\CSVTableOperator.h"
#include "PxcUtil\zPackEx.h"
#include "PxcUtil\LogCenter.h"
#include "PublicDefinitions\SpecialFileDef.h"
#include <string>
#include <hash_map>

#define LOADTABLE(Name, MainPath, Folder, Container, IDMbr)\
{\
	std::string strFile = "DataTables\\";\
	strFile += Folder;\
	strFile += "\\";\
	strFile += #Name;\
	strFile += ".csv";\
	PxcUtil::zPackCombinePath(strFile, MainPath.c_str());\
	strFile = MainPath + strFile;\
	PxcUtil::CCSVTableOperator tabop;\
	if (tabop.Load(strFile.c_str()))\
	{\
		while (tabop.ReadRow())\
		{\
			C##Name##Row* p##Name##Row = new C##Name##Row();\
			p##Name##Row->Read(tabop);\
			Container.insert(std::pair<int, C##Name##Row*>(p##Name##Row->IDMbr, p##Name##Row));\
		}\
		tabop.Reset();\
	}\
	else\
	{\
		PXCU_LOG_ERROR(SpecialFileDef::ELogFile_AssetsTables, "数据表加载失败") << strFile;\
		return false;\
	}\
}

#define UNLOADTABLE(Name, Container)\
{\
	std::map<int, C##Name##Row*>::iterator iter = Container.begin();\
	for (; iter != Container.end(); iter++)\
		delete iter->second;\
	Container.clear();\
}

struct TabColValue
{
	virtual void vfn(){}
	PxcUtil::CCSVTableOperator::ECol_Type eType;
};

struct TabColIntValue : public TabColValue
{
	TabColIntValue();
	int value;
	static int svalue;
};

struct TabColInt64Value : public TabColValue
{
	TabColInt64Value();
	long long value;
	static long long svalue;
};

struct TabColFloatValue : public TabColValue
{
	TabColFloatValue();
	float value;
	static float svalue;
};

struct TabColStringValue : public TabColValue
{
	TabColStringValue();
	std::string value;
	static std::string svalue;
};

struct TabColIntArray : public TabColValue
{
	TabColIntArray();
	std::vector<int> value;
	static std::vector<int> svalue;
};

struct TabColInt64Array : public TabColValue
{
	TabColInt64Array();
	std::vector<long long> value;
	static std::vector<long long> svalue;
};

struct TabColFloatArray : public TabColValue
{
	TabColFloatArray();
	std::vector<float> value;
	static std::vector<float> svalue;
};

struct TabColStringArray : public TabColValue
{
	TabColStringArray();
	std::vector<std::string> value;
	static std::vector<std::string> svalue;
};

#define DECLARE_LEAVECOL_MEMBERS template<typename T, typename TCV>\
T GetLeave(const std::string& strColName)\
{\
	std::hash_map<std::string, TabColValue*>::iterator iter = m_hmapLeaveCols.find(strColName);\
	if (iter != m_hmapLeaveCols.end())\
	{\
		TCV* pDeriveValue = dynamic_cast<TCV*>(iter->second);\
		if (pDeriveValue)\
			return pDeriveValue->value;\
	}\
	return TCV::svalue;\
}\
std::hash_map<std::string, TabColValue*> m_hmapLeaveCols;

#define RELEASE_LEAVECOLS {\
	std::hash_map<std::string, TabColValue*>::iterator iter = m_hmapLeaveCols.begin();\
	for (; iter != m_hmapLeaveCols.end(); iter++)\
		delete iter->second;\
	m_hmapLeaveCols.clear();\
}

#define READ_LEAVECOLS(Tabop)\
{\
	PxcUtil::CCSVTableOperator::ColIter colit = Tabop.Begin();\
	while (colit.Ok())\
	{\
		TabColValue* pValue = NULL;\
		switch (colit.GetType())\
		{\
		case PxcUtil::CCSVTableOperator::ECol_Int:\
			{\
				TabColIntValue* pIntValue = new TabColIntValue();\
				colit.GetValue(pIntValue->value);\
				pValue = pIntValue;\
			}\
			break;\
		case PxcUtil::CCSVTableOperator::ECol_Int64:\
			{\
				TabColInt64Value* pInt64Value = new TabColInt64Value();\
				colit.GetValue(pInt64Value->value);\
				pValue = pInt64Value;\
			}\
			break;\
		case PxcUtil::CCSVTableOperator::ECol_Float:\
			{\
				TabColFloatValue* pFloatValue = new TabColFloatValue();\
				colit.GetValue(pFloatValue->value);\
				pValue = pFloatValue;\
			}\
			break;\
		case PxcUtil::CCSVTableOperator::ECol_String:\
			{\
				TabColStringValue* pStringValue = new TabColStringValue();\
				colit.GetValue(pStringValue->value);\
				pValue = pStringValue;\
			}\
			break;\
		case PxcUtil::CCSVTableOperator::ECol_IntArray:\
			{\
				TabColIntArray* pIntArray = new TabColIntArray();\
				colit.GetArray(pIntArray->value);\
				pValue = pIntArray;\
			}\
			break;\
		case PxcUtil::CCSVTableOperator::ECol_Int64Array:\
			{\
				TabColInt64Array* pInt64Array = new TabColInt64Array();\
				colit.GetArray(pInt64Array->value);\
				pValue = pInt64Array;\
			}\
			break;\
		case PxcUtil::CCSVTableOperator::ECol_FloatArray:\
			{\
				TabColFloatArray* pFloatArray = new TabColFloatArray();\
				colit.GetArray(pFloatArray->value);\
				pValue = pFloatArray;\
			}\
			break;\
		case PxcUtil::CCSVTableOperator::ECol_StringArray:\
			{\
				TabColStringArray* pStringArray = new TabColStringArray();\
				colit.GetArray(pStringArray->value);\
				pValue = pStringArray;\
			}\
			break;\
		}\
		m_hmapLeaveCols.insert(std::make_pair(colit.GetName(), pValue));\
		colit.Next();\
	}\
}

#define LEAVECOL_I int, TabColIntValue
#define LEAVECOL_L long long, TabColInt64Value
#define LEAVECOL_F float, TabColFloatValue
#define LEAVECOL_S std::string, TabColStringValue
#define LEAVECOL_IA std::vector<int>&, TabColIntArray
#define LEAVECOL_LA std::vector<long long>&, TabColInt64Array
#define LEAVECOL_FA std::vector<float>&, TabColFloatArray
#define LEAVECOL_SA std::vector<std::string>&, TabColStringArray