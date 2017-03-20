#pragma once
#include <string>
//这是一个假的zPack

namespace PxcUtil
{

//enum EzPOpen_Result
//{
//	EzPOpen_Ok = 0,
//	EzPOpen_SimplePath,
//	EzPOpen_NoPack,
//	EzPOpen_NoFile,
//};

//EzPOpen_Result zPackFOpen(const char* szComboPath, zp::IReadFile** ppReadFile);
void zPackRelease();
void zPackPathSwitch(bool bFlag, bool bFileFirst = true);
void zPackAddPathAim(const char* szPack, const char* szPath);
bool zPackCombinePath(std::string& strPath);
void zPackClearPathAims();

}