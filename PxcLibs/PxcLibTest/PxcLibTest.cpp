// PxcLibTest.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "PxcUtil\CSVTableOperator.h"
#include "PxcUtil\FileManage.h"
#include "PxcUtil/DateTime.h"
#include "PxcUtil/IDPool.h"
#include "PxcUtil/Random.h"
#include "PxcUtil/LogCenter.h"
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

	std::cout << "=====================" << std::endl;
	CSingleTest::GetInstance()->Do();
	CSingleTest2::GetInstance()->Do();
	CSingleTest::DeleteInstance();
	CSingleTest2::DeleteInstance();

	std::cout << "=====================" << std::endl;
	ExactTime::Init();
	std::cout << "ExactTime: " << ExactTime::GetSysTick();
	std::cout << ", " << ExactTime::GetTime(ExactTime::ELevel_Second);
	std::cout << ", " << ExactTime::GetTime(ExactTime::ELevel_Milli);
	std::cout << ", " << ExactTime::GetTime(ExactTime::ELevel_Micro) << std::endl;
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

	return 0;
}

