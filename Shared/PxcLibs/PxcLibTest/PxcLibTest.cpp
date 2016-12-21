// PxcLibTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "PxcUtil\CSVTableOperator.h"
#include "PxcUtil\FileManage.h"
#include "PxcUtil/DateTime.h"
#include "PxcUtil/IDPool.h"
#include "PxcUtil/Random.h"
#include "PxcUtil/LogCenter.h"
#include "PxcUtil/StringTools.h"
#include "PxcUtil/md5.h"
#include "PxcUtil/IniFile.h"
#include "tinyxml/tinyxml.h"
#include "PxcUtil/zPackEx.h"
#include "PxcUtil/StateMachine.h"
#include "SingletonTest.h"
#include "TestClasses.h"
#include <iostream>

using namespace PxcUtil;
//注释

template<typename T>
void OutputArray(std::vector<T>& vec)
{
	std::cout << "\t[";
	std::vector<T>::iterator iter = vec.begin();
	for (; iter != vec.end(); iter++)
	{
		if (iter == vec.begin())
			std::cout << *iter;
		else
			std::cout << ";" << *iter;
	}
	std::cout << "]";
}

int _tmain(int argc, _TCHAR* argv[])
{
	PXCU_LOGINST->Init(1, "logtest.txt");

	std::cout << "=====================" << std::endl;
	std::string strFile = "test.csv";
	CCSVOperator csvop;
	if (csvop.LoadCSV(strFile.c_str()))
	{
		int i = 1;
		std::string* pstrCol = csvop.GetString(1, i);
		while (pstrCol)
		{
			if (i == 1)
				std::cout << *pstrCol;
			else
				std::cout << "\t" << *pstrCol;
			i++;
			pstrCol = csvop.GetString(1, i);
		}
		std::cout << std::endl;

		int nRowNumEx = csvop.GetCSVMap().size();
		if (nRowNumEx > 1)
		{
			for (int nRow = 2; nRow <= nRowNumEx; ++nRow)
			{
				i = 1;
				pstrCol = csvop.GetString(nRow, i);
				while (pstrCol)
				{
					if (i == 1)
						std::cout << *pstrCol;
					else
						std::cout << "\t" << *pstrCol;
					i++;
					pstrCol = csvop.GetString(nRow, i);
				}
				std::cout << std::endl;
			}
		}
		else
		{
			std::cout << "文件无数据" << std::endl;
		}
	}
	else
	{
		std::cout << strFile << "文件加载失败" << std::endl;
	}

	std::cout << "=====================" << std::endl;
	strFile = "test2.csv";
	CCSVTableOperator tabop;
	if (tabop.Load(strFile.c_str()))
	{
		while (tabop.ReadRow())
		{
			int iID;
			if (tabop.GetType("ID") == CCSVTableOperator::ECol_Int && tabop.GetValue("ID", iID))
				std::cout << iID;

			std::string strName;
			if (tabop.GetType("Name") == CCSVTableOperator::ECol_String && tabop.GetValue("Name", strName))
				std::cout << "\t" << strName;

			float fHaha;
			if (tabop.GetType("Haha") == CCSVTableOperator::ECol_Float && tabop.GetValue("Haha", fHaha))
				std::cout << "\t" << fHaha;

			std::vector<int> vecZu1;
			if (tabop.GetType("Zu1") == CCSVTableOperator::ECol_IntArray && tabop.GetArray("Zu1", vecZu1))
				OutputArray(vecZu1);

			std::vector<std::string> vecZu2;
			if (tabop.GetType("Zu2") == CCSVTableOperator::ECol_StringArray && tabop.GetArray("Zu2", vecZu2))
				OutputArray(vecZu2);

			std::vector<float> vecZu3;
			if (tabop.GetType("Zu3") == CCSVTableOperator::ECol_FloatArray && tabop.GetArray("Zu3", vecZu3))
			{
				OutputArray(vecZu3);
				std::cout << std::endl;
			}
		}

		tabop.AddColumn("ColEx", CCSVTableOperator::ECol_Int, "-1", "扩展列(已有数据行无法扩展)");
		tabop.SetValue("ID", 5);
		tabop.SetValue<std::string>("Name", "E");
		tabop.SetValue("Haha", 5.5f);
		std::vector<int> vecZu1Save;
		vecZu1Save.push_back(5);
		vecZu1Save.push_back(5);
		tabop.SetArray("Zu1", vecZu1Save);
		tabop.WriteRow();
		tabop.Save("test3.csv");
	}
	else
	{
		std::cout << strFile << "文件加载失败" << std::endl;
	}

	std::cout << "=====================" << std::endl;
	std::vector<std::wstring> vecFiles;
	FileManage::FindFilesRecursive(L"testdir", L"txt", vecFiles);
	std::vector<std::wstring>::iterator itFile = vecFiles.begin();
	for (; itFile != vecFiles.end(); itFile++)
	{
		std::wcout << *itFile << std::endl;
	}
	std::wcout << FileManage::GetAbsPath() << std::endl;
	if (FileManage::CreateFolder(L"testdir2\\testdir2in"))
	{
		IniFile inifm;
		inifm.SetValue("section1", "int1", 1);
		inifm.SetPath("testdir2\\testdir2in\\testinifm.ini");
		inifm.Save();
	}

	std::cout << "=====================" << std::endl;
	CSingleTest::GetInstance()->Do();
	CSingleTest2::GetInstance()->Do();
	CSingleTest::DeleteInstance();
	CSingleTest2::DeleteInstance();

	std::cout << "=====================" << std::endl;
	ExactTime::Init();
	std::cout << "ExactTime: " << ExactTime::GetSysTick();
	std::cout << ", " << ExactTime::GetPerfTime(ExactTime::ELevel_Second);
	std::cout << ", " << ExactTime::GetPerfTime(ExactTime::ELevel_Milli);
	std::cout << ", " << ExactTime::GetPerfTime(ExactTime::ELevel_Micro);
	float fTime = ExactTime::GetFloatTime();
	std::cout << ", " << fTime << std::endl;
	DateTimeInfo info;
	long long lDateTime = DateTime::GetDateTime();
	std::cout << "DateTime: " << lDateTime;
	if (DateTime::InformDateTime(lDateTime, info))
	{
		std::cout << " " << info.date.getYear() << "年" << info.date.getMonth() << "月" << info.date.getDay() << "日 ";
		std::cout << info.iHour << ":" << info.iMin << ":" << info.iSec << " " << info.date.getWeekday() << " " << info.date.getDaysPassedOfYear();
		/*
		info.date.addDays(-2);
		std::cout << " " << info.date.getWeekday();
		//*/
		std::cout << " " << DateTime::IntegrateDateTime(info);

		FILE* pTimeFile = FileManage::OpenFileWithCreate(L"testdir3\\testtimefm.txt");
		if (pTimeFile)
		{
			char szBuf[128];
			sprintf(szBuf, "%d:%d:%d", info.iHour, info.iMin, info.iSec);
			fwrite(szBuf, strlen(szBuf), 1, pTimeFile);
			fclose(pTimeFile);
		}
	}
	std::cout << std::endl;

	std::cout << "=====================" << std::endl;
	CSubRun subRun;
	CThread subThread(&subRun);
	subThread.Start();
	for (int i = 0; i < 5; ++i)
	{
		std::cout << "MainThread Count " << i << std::endl;
		Sleep(1000);
	}

	std::cout << "=====================" << std::endl;
	CIDPool idpool(0, 10, 3);
	idpool.Declare(3);
	idpool.Declare(5);
	idpool.Declare(9);
	std::cout << "ID Generate : ";//0, 1, 2, 4, 6, 1, 2, 7, 8, 10
	for (int i = 0; i < 5; ++i)
	{
		if (i != 0)
			std::cout << ", ";
		std::cout << idpool.Generate();
	}
	idpool.Free(1);
	idpool.Free(2);
	int iGen = idpool.Generate();
	while (iGen != 3)
	{
		std::cout << ", " << iGen;
		iGen = idpool.Generate();
	}
	std::cout << std::endl;

	std::cout << "=====================" << std::endl;
	std::cout << "RandomInt : " << Random::RandInt(-10, 10) << ", " << Random::RandInt(0, 65536) << ", " << Random::RandInt(0, 65544) << "; ";
	Sleep(2000);
	Random::SetReseedCycle(4);
	std::cout << Random::RandInt(-10, 10) << ", " << Random::RandInt(0, 65536) << ", " << Random::RandInt(0, 65544) << std::endl;
	std::cout << "RandomFloat : " << Random::RandFloat(0.1f, 0.5f) << ", " << Random::RandFloat(-10.0f, 10.0f) << ", " <<
		Random::RandFloat(0.1f, 0.5f) << ", " << Random::RandFloat(-10.0f, 10.0f) << ", " <<
		Random::RandFloat(0.000001f, 0.0000015f) << ", " << Random::RandFloat(0.0f, 66666.6f) << std::endl;
	{
		std::vector<int> vecNormLots;
		for (int i = 0; i < 10; ++i)
			vecNormLots.push_back(i);
		std::cout << "DrawLots : " << Random::DrawLots(vecNormLots);

		std::vector<std::pair<int, float>> vecProbLots;
		vecProbLots.push_back(std::make_pair(0, -1.0f));
		vecProbLots.push_back(std::make_pair(1, 0.5f));
		vecProbLots.push_back(std::make_pair(2, 0.5f));
		vecProbLots.push_back(std::make_pair(3, 0.1f));
		std::cout << ", " << Random::DrawLots(vecProbLots);

		std::vector<int> vecResult;
		bool bRet = Random::DrawLots(vecNormLots, 3, vecResult);
		std::cout << ",";
		for (int i = 0, iSize = vecResult.size(); i < iSize; ++i)
			std::cout << " " << vecResult[i];

		vecResult.clear();
		bRet = Random::DrawLots(vecProbLots, 2, vecResult);
		std::cout << ",";
		for (int i = 0, iSize = vecResult.size(); i < iSize; ++i)
			std::cout << " " << vecResult[i];
		std::cout << std::endl;
	}

	std::cout << "=====================" << std::endl;
	PXCU_LOG_DEBUG(1) << "debug";
	PXCU_LOG(1) << "info";
	PXCU_LOG_WARNING(1, "warning") << "warning";
	PXCU_LOG_ASSERT(1, 1 == 1) << "true";
	PXCU_LOG_ASSERT(1, 1 == 2) << "false";
	PXCU_LOG_ERROR(1, "error") << "error";
	PXCU_LOGINST->Release();

	std::cout << "=====================" << std::endl;
	std::string strStrTest = StringTools::Trim(StringTools::Format("  1%d1%.2f\t", 1, 1.222f));
	float fStrTest = StringTools::StrToBasic<float>(strStrTest);
	strStrTest = StringTools::BasicToStr(fStrTest);
	std::cout << strStrTest << std::endl;
	std::wstring wstrStrTest = StringTools::Trim(StringTools::Format(L"  1%d1%.2f\t", 1, 1.222f));
	fStrTest = StringTools::WstrToBasic<float>(wstrStrTest);
	wstrStrTest = StringTools::BasicToWstr(fStrTest);
	strStrTest = StringTools::WstrToStr(wstrStrTest);
	std::cout << strStrTest << std::endl;
	std::cout << StringTools::Format("", 1, 2) << std::endl;

	std::cout << "=====================" << std::endl;
	CMd5 md5;
	md5.TargetStr("sb");
	std::cout << md5.GetDigestKey() << std::endl;

	std::cout << "=====================" << std::endl;
	IniFile ini;
	ini.SetValue("section1", "int1", 1);
	ini.SetValue("section1", "string1", "test");
	ini.SetValue("section2", "int2", 2);
	ini.SetValue("section2", "string2", "test");
	ini.SetPath("testini.ini");
	ini.Save();
	ini.ClearAll();
	ini.Load("testini.ini");
	bool bValue = 0;
	if (ini.GetValue("section1", "int1", bValue))
		std::cout << "INI:" << (bValue ? "true" : "false") << std::endl;

	std::cout << "=====================" << std::endl;
	if (FileManage::RemoveFile(L"testdir2\\testdir2in\\testinifm.ini"))
		std::cout << "testdir2\\testdir2in\\testinifm.ini 删除成功" << std::endl;
	if (FileManage::RemoveFolder(L"testdir2\\testdir2in"))
		std::cout << "testdir2\\testdir2in 删除成功" << std::endl;
	if (FileManage::RemoveFolder(L"testdir2"))
		std::cout << "testdir2 删除成功" << std::endl;

	std::cout << "==========XML==========" << std::endl;
	TiXmlDocument doc;
	if (doc.LoadFile("testxml.xml"))
	{
		TiXmlElement* pRoot = doc.RootElement();
		if (pRoot)
		{
			int iValue = 0;
			TiXmlElement* pItem = pRoot->FirstChildElement("ItemInt");
			while (pItem)
			{
				pItem->QueryIntAttribute("Value", &iValue);
				std::cout << iValue << std::endl;
				pItem = pItem->NextSiblingElement("ItemInt");
			}
			pItem = pRoot->FirstChildElement("ItemStr");
			while (pItem)
			{
				std::string strValue = pItem->Attribute("Value");
				std::cout << strValue << std::endl;
				pItem = pItem->NextSiblingElement("ItemStr");
			}
			TiXmlElement eleNew("ItemInt");
			eleNew.SetAttribute("Value", iValue + 1);
			pRoot->InsertEndChild(eleNew);
			doc.SaveFile("testxml2.xml");
		}
	}

	std::cout << "==========zpack==========" << std::endl;
	tabop.Reset();
	if (tabop.Load("testzpk.zpk#dir\\test2.csv"))
	{
		tabop.ReadRow();
		std::string strName;
		if (tabop.GetValue("Name", strName))
			std::cout << strName << std::endl;
	}
	doc.Clear();
	if (doc.LoadFile("testzpk.zpk#dir\\testxml.xml"))
	{
		TiXmlElement* pRoot = doc.RootElement();
		if (pRoot)
		{
			int iValue = 0;
			TiXmlElement* pItem = pRoot->FirstChildElement("ItemInt");
			if (pItem)
			{
				pItem->QueryIntAttribute("Value", &iValue);
				std::cout << iValue << std::endl;
			}
		}
	}
	zPackRelease();

	std::cout << "==========delegate==========" << std::endl;
	CDali dali;
	tDaliGate::FuncObjSta funcObjSta(&CDali::StaFunc);
	tDaliGate::FuncObjDyn<CDali> funcObjDyn(&CDali::DynFunc, &dali);
	tDaliGate daliGate;
	daliGate.Register(&funcObjSta);
	daliGate.Register(&funcObjDyn);
	std::cout << "EventReturn : " << (daliGate(1) ? 1 : 0) << std::endl;
	daliGate.UnRegister(&funcObjDyn);
	std::cout << "EventReturn : " << (daliGate(2) ? 1 : 0) << std::endl;

	std::cout << "==========StateMachine==========" << std::endl;
	std::vector<std::string> vecStrs;
	vecStrs.push_back("A");
	vecStrs.push_back("B");
	vecStrs.push_back("C");
	CStateMachine<std::string, int> fsm;
	std::vector<std::string>::iterator iter = vecStrs.begin();
	for (; iter != vecStrs.end(); iter++)
	{
		fsm.AddState(&(*iter));
	}
	iter = vecStrs.begin();
	int i = 0;
	for (; iter != vecStrs.end(); iter++)
	{
		std::vector<std::string>::iterator iter2 = iter + 1;
		if (iter2 == vecStrs.end())
			iter2 = vecStrs.begin();
		fsm.AddTransfer(&(*iter), i, &(*iter2));
		i++;
	}
	if (fsm.SetState(&vecStrs[0], true))
		std::cout << "Init OK, Current State : " << *fsm.GetCurrentState() << std::endl;
	if (fsm.SetState(&vecStrs[1]))
		std::cout << "Set OK, Current State : " << *fsm.GetCurrentState() << std::endl;
	if (!fsm.SetState(&vecStrs[0]))
		std::cout << "Set Fail, Current State : " << *fsm.GetCurrentState() << std::endl;
	if (fsm.TriggerEvent(1))
		std::cout << "Transfer Ok, Current State : " << *fsm.GetCurrentState() << std::endl;
	if (!fsm.TriggerEvent(1))
		std::cout << "Transfer Fail, Current State : " << *fsm.GetCurrentState() << std::endl;
	fsm.Clear();

	return 0;
}

