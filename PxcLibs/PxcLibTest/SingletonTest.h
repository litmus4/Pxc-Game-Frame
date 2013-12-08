#pragma once
#include "PxcUtil/Singleton.h"
#include <iostream>

class CSingleTest : public PxcUtil::Singleton<CSingleTest>
{
DEFINE_SINGLETON_CONSTRUCTOR(CSingleTest) : m_iData(2) {}
public:
	void Do()
	{
		std::cout << "单件!" << m_iData << std::endl;
	}

private:
	int m_iData;
};

class CSingleTest2 : public PxcUtil::Singleton<CSingleTest2>
{
DEFINE_SINGLETON_CONSTRUCTOR(CSingleTest2) : m_iData(4) {}
public:
	void Do()
	{
		std::cout << "单件2!" << m_iData << std::endl;
	}

private:
	int m_iData;
};