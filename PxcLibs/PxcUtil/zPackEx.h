#pragma once
#include "zpack/zpack.h"
#include <map>

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

}