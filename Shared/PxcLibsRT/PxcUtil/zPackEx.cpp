#include "zPackEx.h"
#include "StringTools.h"
#include <map>
//这是一个假的zPack

using namespace Windows::Storage;

namespace PxcUtil
{

enum EUwpPath_Type
{
	EUwpPath_Install = 0,
	EUwpPath_Local,
	EUwpPath_Roaming,
	EUwpPath_Temp,
};
std::map<std::string, EUwpPath_Type> g_mapUwpPathAims;
std::string g_strDeployFolder;

void zPackRelease()
{
	//
}

void zPackPathSwitch(bool bFlag, bool bFileFirst)
{
	//
}

void zPackAddPathAim(const char* szPack, const char* szPath)
{
	std::string strPath = szPath;
	int ipos = strPath.find_first_of('\\');
	if (ipos != std::string::npos)
		strPath = strPath.substr(0, ipos);

	std::string strUri = szPack;
	ipos = strUri.find(":///");
	if (ipos != std::string::npos)
	{
		std::string strUriHead = strUri.substr(0, ipos);
		std::string strUriSub = strUri.substr(ipos + 4);
		EUwpPath_Type eType = EUwpPath_Install;
		if (strUriHead == "ms-appx")
		{
			if (g_strDeployFolder.empty())
				g_strDeployFolder = strUriSub;
		}
		else if (strUriHead == "ms-appdata")
		{
			if (strUriSub == "local")
				eType = EUwpPath_Local;
			else if (strUriSub == "roaming")
				eType = EUwpPath_Roaming;
			else if (strUriSub == "temporary")
				eType = EUwpPath_Temp;
		}

		g_mapUwpPathAims.insert(std::make_pair(strPath, eType));
	}
}

bool zPackCombinePath(std::string& strPath)
{
	std::string strPathKey = strPath;
	int ipos = strPathKey.find_first_of('\\');
	if (ipos != std::string::npos)
		strPathKey = strPathKey.substr(0, ipos);
	else
	{
		ipos = strPathKey.find_first_of('.');
		if (ipos != std::string::npos)
		{
			Platform::String^ pstrUwpPath = ApplicationData::Current->LocalFolder->Path;
			std::string strUwpPath = StringTools::WstrToStr(pstrUwpPath->Data());
			strPath = strUwpPath + "\\" + strPath;
			return true;
		}
	}

	std::map<std::string, EUwpPath_Type>::iterator iter = g_mapUwpPathAims.find(strPathKey);
	if (iter != g_mapUwpPathAims.end())
	{
		Platform::String^ pstrUwpPath = ref new Platform::String(L"");
		std::string strUwpPath;
		switch (iter->second)
		{
		case EUwpPath_Install:
			{
				pstrUwpPath = Windows::ApplicationModel::Package::Current->InstalledLocation->Path;
				strUwpPath = StringTools::WstrToStr(pstrUwpPath->Data()) + "\\" + g_strDeployFolder;
			}
			break;
		case EUwpPath_Local:
			{
				pstrUwpPath = ApplicationData::Current->LocalFolder->Path;
				strUwpPath = StringTools::WstrToStr(pstrUwpPath->Data());
			}
			break;
		case EUwpPath_Roaming:
			{
				pstrUwpPath = ApplicationData::Current->RoamingFolder->Path;
				strUwpPath = StringTools::WstrToStr(pstrUwpPath->Data());
			}
			break;
		case EUwpPath_Temp:
			{
				pstrUwpPath = ApplicationData::Current->TemporaryFolder->Path;
				strUwpPath = StringTools::WstrToStr(pstrUwpPath->Data());
			}
			break;
		}
		strPath = strUwpPath + "\\" + strPath;
		return true;
	}
	return false;
}

void zPackClearPathAims()
{
	g_mapUwpPathAims.clear();
}

}