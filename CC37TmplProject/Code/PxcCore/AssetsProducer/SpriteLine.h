#pragma once
#include "cocos2d.h"
#include "BaseLine.h"

class CSpriteProduct;
class CSpriteLine : public CBaseLine
{
public:
	CSpriteLine();
	virtual ~CSpriteLine();

	virtual bool Init(const char* szFileName);
	CSpriteProduct* Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn = false);

	void DrawLoading(cocos2d::Vec4& v4Trans, cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags);
	void DiscardLoadingDraw();

protected:
	virtual CBaseProduct* CreateProduct();

private:
	CSpriteProduct* m_pLoadingDraw;
};