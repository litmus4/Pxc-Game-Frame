#pragma once
#include <stdio.h>
#include <windows.h>
#include <vector>

namespace PxcUtil
{


namespace FileManage
{

BOOL IsRoot(LPCTSTR lpszPath);
void FindFilesRecursive(LPCTSTR lpszPath, const wchar_t* szExt, std::vector<std::wstring>& vecOut);
bool IsFileExist(LPCTSTR lpszFileName);

}


}