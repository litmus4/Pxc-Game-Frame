#pragma once
#include "zpack/zpack.h"

#define zPackFClose(rfile) rfile->deleteMe()

namespace PxcUtil
{

enum EzPOpen_Result
{
	EzPOpen_Ok = 0,
	EzPOpen_SimplePath,
	EzPOpen_NoPack,
	EzPOpen_NoFile,
};

EzPOpen_Result zPackFOpen(const char* szComboPath, zp::IReadFile** ppReadFile);
void zPackRelease();
void zPackPathSwitch(bool bFlag, bool bFileFirst = true);
void zPackAddPathAim(const char* szPack, const char* szPath);
bool zPackCombinePath(std::string& strPath, const char* szLocPath = NULL);
void zPackClearPathAims();

}