#include "stdafx.h"
#include "TestClasses.h"
#include <iostream>

void CSubRun::Run()
{
	for (int i = 0; i < 5; ++i)
	{
		std::cout << "SubThread Count " << i << std::endl;
		Sleep(1000);
	}
}

CDali::CDali()
{
	//
}

CDali::~CDali()
{
	//
}

bool CDali::DynFunc(int i)
{
	std::cout << "CDali::DynFunc arg=" << i << std::endl;
	return true;
}

bool CDali::StaFunc(int i)
{
	std::cout << "CDali::StaFunc arg=" << i << std::endl;
	return false;
}