#include "FileManage.h"

namespace PxcUtil
{


namespace FileManage
{

BOOL IsRoot(LPCTSTR lpszPath)
{
	TCHAR szRoot[4];
	wsprintf(szRoot, L"%c:\\", lpszPath[0]);
	return (lstrcmp(szRoot, lpszPath) == 0);
}

void FindFilesRecursive(LPCTSTR lpszPath, const wchar_t* szExt, std::vector<std::wstring>& vecOut)
{
	TCHAR szFind[MAX_PATH];
	lstrcpy(szFind, lpszPath);
	if (!IsRoot(szFind))
		lstrcat(szFind, L"\\");
	lstrcat(szFind, L"*.*"); // 找所有文件
	int iExtLen = lstrlen(szExt);

	WIN32_FIND_DATA wfd;
	HANDLE hFind = FindFirstFileExW(szFind, FindExInfoStandard, &wfd, FindExSearchNameMatch, NULL, 0);
	if (hFind == INVALID_HANDLE_VALUE) // 如果没有找到或查找失败
		return;
	do
	{
		if (wfd.cFileName[0] == '.')
			continue; // 过滤这两个目录
		if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			TCHAR szFile[MAX_PATH];
			if (IsRoot(lpszPath))
				wsprintf(szFile, L"%s%s", lpszPath, wfd.cFileName);
			else
				wsprintf(szFile, L"%s\\%s", lpszPath, wfd.cFileName);
			FindFilesRecursive(szFile, szExt, vecOut); // 如果找到的是目录，则进入此目录进行递归
		}
		else
		{
			std::wstring wstrFileName = wfd.cFileName;
			int iFileLen = wstrFileName.size();
			if (!szExt || iExtLen == 0 ||
				(iFileLen > iExtLen + 1 && wstrFileName.substr(iFileLen - iExtLen, iExtLen).compare(szExt) == 0))
			{
				TCHAR szFile[MAX_PATH];
				if (IsRoot(lpszPath))
					wsprintf(szFile, L"%s%s", lpszPath, wfd.cFileName);
				else
					wsprintf(szFile, L"%s\\%s", lpszPath, wfd.cFileName);
				//printf("%s\n",szFile);
				vecOut.push_back(szFile);
			}
		}
	} while (FindNextFile(hFind, &wfd));
	FindClose(hFind); // 关闭查找句柄

}

}


}