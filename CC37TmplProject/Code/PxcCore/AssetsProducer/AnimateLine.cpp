#include "AnimateProduct.h"
#include "AnimateLine.h"

CAnimateLine::CAnimateLine()
{
	//
}

CAnimateLine::~CAnimateLine()
{
	//
}

CAnimateProduct* CAnimateLine::Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn)
{
	return static_cast<CAnimateProduct*>(FetchProduct(iID, eLoadType, bAsyn));
}

CBaseProduct* CAnimateLine::CreateProduct()
{
	return new CAnimateProduct();
}