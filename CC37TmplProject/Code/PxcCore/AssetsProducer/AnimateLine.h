#pragma once
#include "BaseLine.h"

class CAnimateProduct;
class CAnimateLine : public CBaseLine
{
public:
	CAnimateLine();
	virtual ~CAnimateLine();

	CAnimateProduct* Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn = false);

protected:
	virtual CBaseProduct* CreateProduct();
};