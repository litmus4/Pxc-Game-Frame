#include "zPackEx.h"
#include "StringTools.h"

namespace PxcUtil
{

std::map<std::string, zp::IPackage*> g_mapOpenZPacks;

zp::IReadFile* zPackFOpen(const char* szComboPath)
{
	std::string strComboPath = szComboPath;
	int ipos = strComboPath.find_first_of('#');
	if (ipos == std::string::npos)
		return NULL;
	std::string strPack = strComboPath.substr(0, ipos);
	std::string strFile = strComboPath.substr(ipos + 1, strComboPath.size() - ipos - 1);

	zp::IPackage* pPackage = NULL;
	std::map<std::string, zp::IPackage*>::iterator iter = g_mapOpenZPacks.find(strPack);
	if (iter == g_mapOpenZPacks.end())
	{
		pPackage = zp::open(PxcUtil::StringTools::StrToWstr(strPack).c_str(), zp::OPEN_READONLY);
		if (pPackage)
			g_mapOpenZPacks.insert(std::make_pair(strPack, pPackage));
	}
	else
		pPackage = iter->second;
	if (pPackage)
	{
		zp::IReadFile* pFile = pPackage->openFile(PxcUtil::StringTools::StrToWstr(strFile).c_str());
		return pFile;
	}
	return NULL;
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

}