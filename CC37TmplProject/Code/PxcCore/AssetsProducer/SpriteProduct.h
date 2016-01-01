#pragma once
#include "BaseProduct.h"

namespace cocos2d
{
	class Sprite;
}
class CSpriteProduct : public CBaseProduct
{
public:
	CSpriteProduct();
	virtual ~CSpriteProduct();

	bool IsSpriteFrame();
	std::string GetName();

	virtual bool Read(PxcUtil::CCSVTableOperator& tabop);
	virtual bool Load();
	virtual void UnLoad();
	virtual CBaseProduct* Clone();

	void Update(float dt);
	void Draw();
	cocos2d::Sprite* GetSprite();

	virtual void OnLoadComplete();

private:
	std::string m_strName;
	bool m_bSpriteFrame;
	std::string m_strPlistFileName;
	std::string m_strAtlasFileName;
	CSpriteProduct* m_pTmpl;

	cocos2d::Sprite* m_pSprite;
};