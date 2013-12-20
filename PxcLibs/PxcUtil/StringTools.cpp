#include "StringTools.h"
#include <windows.h>

namespace PxcUtil
{


namespace StringTools
{

std::wstring StrToWstr(const std::string& str)
{
	std::wstring wstrRet;
	if (str.empty())
		return wstrRet;
	int iSize = MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
	if (iSize != 0)
	{
		LPWSTR wszBuf = new WCHAR[iSize];
		MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszBuf, iSize);
		wstrRet = wszBuf;
		delete []wszBuf;
	}
	return wstrRet;
}

}


}