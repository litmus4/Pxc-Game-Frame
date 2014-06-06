#include "zPackEx.h"
#include "StringTools.h"
#include "FileManage.h"
#include <map>

namespace PxcUtil
{

std::map<std::string, zp::IPackage*> g_mapOpenZPacks;
bool g_bZPackPath = false;
bool g_bZPackPathFileFirst = true;
std::map<std::string, std::string> g_mapZPackPathAims;

EzPOpen_Result zPackFOpen(const char* szComboPath, zp::IReadFile** ppReadFile)
{
	std::string strComboPath = szComboPath;
	int ipos = strComboPath.find_first_of('#');
	if (ipos == std::string::npos)
		return EzPOpen_SimplePath;
	std::string strPack = strComboPath.substr(0, ipos);
	std::string strFile = strComboPath.substr(ipos + 1, strComboPath.size() - ipos - 1);

	zp::IPackage* pPackage = NULL;
	std::map<std::string, zp::IPackage*>::iterator iter = g_mapOpenZPacks.find(strPack);
	if (iter == g_mapOpenZPacks.end())
	{
		pPackage = zp::open(StringTools::StrToWstr(strPack).c_str(), zp::OPEN_READONLY);
		if (pPackage)
			g_mapOpenZPacks.insert(std::make_pair(strPack, pPackage));
	}
	else
		pPackage = iter->second;
	if (pPackage)
	{
		zp::IReadFile* pFile = pPackage->openFile(StringTools::StrToWstr(strFile).c_str());
		if (pFile)
		{
			*ppReadFile = pFile;
			return EzPOpen_Ok;
		}
		else
			return EzPOpen_NoFile;
	}
	return EzPOpen_NoPack;
}

void zPackRelease()
{
	std::map<std::string, zp::IPackage*>::iterator iter = g_mapOpenZPacks.begin();
	for (; iter != g_mapOpenZPacks.end(); iter++)
	{
		zp::close(iter->second);
	}
	g_mapOpenZPacks.clear();
}

void zPackPathSwitch(bool bFlag, bool bFileFirst)
{
	g_bZPackPath = bFlag;
	g_bZPackPathFileFirst = bFileFirst;
}

void zPackAddPathAim(const char* szPack, const char* szPath)
{
	if (!g_bZPackPath)
		return;

	std::string strPath = szPath;
	int ipos = strPath.find_first_of('\\');
	if (ipos != std::string::npos)
		strPath = strPath.substr(0, ipos);

	g_mapZPackPathAims.insert(std::pair<std::string, std::string>(strPath, szPack));
}

void zPackCombinePath(std::string& strPath)
{
	if (!g_bZPackPath)
		return;
	if (g_bZPackPathFileFirst)
	{
		if (FileManage::IsFileExist(StringTools::StrToWstr(strPath).c_str()))
			return;
	}

	std::string strPathKey = strPath;
	int ipos = strPathKey.find_first_of('\\');
	if (ipos != std::string::npos)
		strPathKey = strPathKey.substr(0, ipos);

	std::map<std::string, std::string>::iterator iter = g_mapZPackPathAims.find(strPathKey);
	if (iter != g_mapZPackPathAims.end())
		strPath = iter->second + "#" + strPath;
}

void zPackClearPathAims()
{
	g_mapZPackPathAims.clear();
}

}