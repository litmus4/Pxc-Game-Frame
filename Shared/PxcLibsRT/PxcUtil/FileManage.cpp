#include "FileManage.h"
#include "StringTools.h"

namespace PxcUtil
{


namespace FileManage
{

std::wstring GetAbsPath()
{
	wchar_t wszPath[MAX_PATH];
	memset(wszPath, 0, sizeof(wszPath));
	//GetCurrentDirectoryW(MAX_PATH, wszPath);
	return wszPath;
}

BOOL IsRoot(LPCTSTR lpszPath)
{
	//char szRoot[4];
	//std::string strPath = StringTools::WstrToStr(lpszPath);
	//sprintf_s(szRoot, "%c:\\", strPath.c_str());
	//return (strcmp(szRoot, strPath.c_str()) == 0);
	return (wcslen(lpszPath) == 0);
}

void FindFilesRecursive(LPCTSTR lpszPath, const wchar_t* szExt, std::vector<std::wstring>& vecOut)
{
	char szFind[MAX_PATH];
	strcpy_s(szFind, StringTools::WstrToStr(lpszPath).c_str());
	if (!IsRoot(lpszPath))
		strcat_s(szFind, "\\");
	strcat_s(szFind, "*.*"); // 找所有文件
	int iExtLen = StringTools::WstrToStr(szExt).size();

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFileExW(StringTools::StrToWstr(szFind).c_str(), FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return;
	do
	{
		if (wfd.cFileName[0] == '.')
			continue; // 过滤这两个目录

		std::string strPath = StringTools::WstrToStr(lpszPath);
		std::string strFileName = StringTools::WstrToStr(wfd.cFileName);

		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			char szFile[MAX_PATH];
			if (IsRoot(lpszPath))
				sprintf_s(szFile, "%s%s", strPath.c_str(), strFileName.c_str());
			else
				sprintf_s(szFile, "%s\\%s", strPath.c_str(), strFileName.c_str());
			FindFilesRecursive(StringTools::StrToWstr(szFile).c_str(), szExt, vecOut); // 如果找到的是目录，则进入此目录进行递归
		}
		else
		{
			std::wstring wstrFileName = wfd.cFileName;
			int iFileLen = wstrFileName.size();
			if (!szExt || iExtLen == 0 ||
				(iFileLen > iExtLen + 1 && wstrFileName.substr(iFileLen - iExtLen, iExtLen).compare(szExt) == 0))
			{
				char szFile[MAX_PATH];
				if (IsRoot(lpszPath))
					sprintf_s(szFile, "%s%s", strPath.c_str(), strFileName.c_str());
				else
					sprintf_s(szFile, "%s\\%s", strPath.c_str(), strFileName.c_str());
				//printf("%s\n",szFile);
				vecOut.push_back(StringTools::StrToWstr(szFile));
			}
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind); // 关闭查找句柄

}

bool IsFileExist(LPCTSTR lpszFileName)
{
	WIN32_FIND_DATA wfd;
	HANDLE hFile = FindFirstFileExW(lpszFileName, FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	FindClose(hFile);
	return true;
}

FILE* OpenFileWithCreate(LPCTSTR lpszFileName)
{
	std::wstring wstrFolder = lpszFileName;
	int ipos = wstrFolder.find_last_of(L'\\');
	if (ipos != std::wstring::npos)
	{
		if (!CreateFolder(wstrFolder.substr(0, ipos).c_str()))
			return NULL;
	}
	FILE* pf = NULL;
	_wfopen_s(&pf, lpszFileName, L"w");
	return pf;
}

bool RemoveFile(LPCTSTR lpszFileName)
{
	if (DeleteFileW(lpszFileName))
		return true;
	return false;
}

bool CreateFolder(LPCTSTR lpszPath)
{
	if (!CreateDirectoryW(lpszPath, NULL))
	{
		DWORD dwError = GetLastError();
		if (dwError == ERROR_PATH_NOT_FOUND)
		{
			std::wstring wstrParent = lpszPath;
			int iParentLen = wstrParent.find_last_of(L'\\');
			if (iParentLen != std::wstring::npos)
			{
				if (CreateFolder(wstrParent.substr(0 , iParentLen).c_str()))
					return (bool)CreateDirectoryW(lpszPath, NULL);
				else
					return false;
			}
			else
				return false;
		}
		else if (dwError == ERROR_ALREADY_EXISTS)
			return true;
		else
			return false;
	}
	return true;
}

bool RemoveFolder(LPCTSTR lpszPath)
{
	if (RemoveDirectoryW(lpszPath))
		return true;
	return false;
}

}


}