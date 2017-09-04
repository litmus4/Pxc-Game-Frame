#include "SpriteProduct.h"
#include "SpriteLine.h"
#include "PublicDefinitions/CoreConstantsDef.h"

#define SPRITE_LOADING_DRAW_ID CoreConstantsDef::Assets_Consts::c_iAssets_LoadingDrawID

CSpriteLine::CSpriteLine()
{
	m_pLoadingDraw = NULL;
}

CSpriteLine::~CSpriteLine()
{
	//
}

bool CSpriteLine::Init(const char* szFileName, GlobalDef::ELanguage eLanguage)
{
	bool bRet = CBaseLine::Init(szFileName, eLanguage);
	m_pLoadingDraw = Fetch(SPRITE_LOADING_DRAW_ID, CBaseProduct::ESharedProduct);
	return bRet;
}

CSpriteProduct* CSpriteLine::Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn)
{
	CSpriteProduct* pProduct = static_cast<CSpriteProduct*>(FetchProduct(iID, eLoadType, bAsyn));
	if (pProduct)
		pProduct->SetLine(this);
	return pProduct;
}

void CSpriteLine::DrawLoading(cocos2d::Vec4& v4Trans, cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags)
{
	if (!m_pLoadingDraw)
		return;
	m_pLoadingDraw->SetTransform(v4Trans);
	m_pLoadingDraw->Draw(renderer, parentTrans, parentFlags);
}

void CSpriteLine::DiscardLoadingDraw()
{
	Discard(m_pLoadingDraw);
}

CBaseProduct* CSpriteLine::CreateProduct()
{
	return new CSpriteProduct();
}