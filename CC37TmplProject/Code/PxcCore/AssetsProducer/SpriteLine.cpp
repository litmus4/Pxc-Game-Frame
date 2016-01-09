#include "SpriteProduct.h"
#include "SpriteLine.h"

CSpriteLine::CSpriteLine()
{
	//
}

CSpriteLine::~CSpriteLine()
{
	//
}

CSpriteProduct* CSpriteLine::Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn)
{
	return static_cast<CSpriteProduct*>(FetchProduct(iID, eLoadType, bAsyn));
}

CBaseProduct* CSpriteLine::CreateProduct()
{
	return new CSpriteProduct();
}