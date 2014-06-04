#pragma once
#include "PxcUtil\CSVTableOperator.h"
#include "PxcUtil\zPackEx.h"
#include "PxcUtil\LogCenter.h"
#include "PublicDefinitions\SpecialFileDef.h"
#include <string>

#define LOADTABLE(Name, Folder, Container, IDMbr)\
{\
	std::string strFile = "DataTables\\";\
	strFile += Folder;\
	strFile += "\\";\
	strFile += #Name;\
	strFile += ".csv";\
	PxcUtil::zPackCombinePath(strFile);\
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
		PXCU_LOG_ERROR(SpecialFileDef::ELogFile_AssetsTables, "数据表加载失败 ") << strFile;\
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