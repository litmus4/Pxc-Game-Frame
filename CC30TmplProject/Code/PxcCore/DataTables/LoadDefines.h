#pragma once
#include "PxcUtil\CSVTableOperator.h"
#include <string>

//TODO zpackÓ³Éä
#define LOADTABLE(Name, Folder, IDMbr, Container)\
	std::string strFile = "DataTables\\";\
	strFile += Folder;\
	strFile += "\\";\
	strFile += #Name;\
	strFile += ".csv";\
	PxcUtil::CCSVTableOperator tabop;\
	if (tabop.Load(strFile.c_str()))\
	{\
		while (tabop.ReadRow())\
		{\
			C##Name##Row* p##Name##Row = new C##Name##Row();\
			p##Name##Row->Init(tabop);\
			Container.insert(std::make_pair(p##Name##Row->IDMbr, p##Name##Row));\
		}\
	}