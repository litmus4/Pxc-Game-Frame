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