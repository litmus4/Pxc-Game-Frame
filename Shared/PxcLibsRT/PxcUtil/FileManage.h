#pragma once
#include <stdio.h>
#include <windows.h>
#include <vector>
#include <string>

namespace PxcUtil
{


namespace FileManage
{

std::wstring GetAbsPath();
BOOL IsRoot(LPCTSTR lpszPath);
void FindFilesRecursive(LPCTSTR lpszPath, const wchar_t* szExt, std::vector<std::wstring>& vecOut);
bool IsFileExist(LPCTSTR lpszFileName);
FILE* OpenFileWithCreate(LPCTSTR lpszFileName);
bool RemoveFile(LPCTSTR lpszFileName);
bool CreateFolder(LPCTSTR lpszPath);
bool RemoveFolder(LPCTSTR lpszPath);

}


}