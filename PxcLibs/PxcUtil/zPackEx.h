#pragma once
#include "zpack/zpack.h"
#include <map>

#define zPackFClose(rfile) rfile->deleteMe()

namespace PxcUtil
{

zp::IReadFile* zPackFOpen(const char* szComboPath);
void zPackRelease();

}