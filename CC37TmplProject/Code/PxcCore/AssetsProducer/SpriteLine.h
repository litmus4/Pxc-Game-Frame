#pragma once
#include "BaseLine.h"

class CSpriteProduct;
class CSpriteLine : public CBaseLine
{
public:
	CSpriteLine();
	virtual ~CSpriteLine();

	CSpriteProduct* Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn = false);

protected:
	virtual CBaseProduct* CreateProduct();
};