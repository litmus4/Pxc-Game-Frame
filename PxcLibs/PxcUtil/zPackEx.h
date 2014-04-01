#pragma once
#include "zpack/zpack.h"
#include <map>

namespace PxcUtil
{

zp::IReadFile* zPackFOpen(const char* szComboPath);

}