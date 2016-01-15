#pragma once
#include "cocos2d.h"
#include "BaseProduct.h"

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
	void Draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags);
	cocos2d::Sprite* GetSprite();
	void SetTransform(cocos2d::Vec4& v4Trans, bool bForceStopAct = false);
	cocos2d::Vec4 GetTransform();

	virtual void OnLoadComplete();

private:
	std::string m_strName;
	bool m_bSpriteFrame;
	std::string m_strPlistFileName;
	std::string m_strAtlasFileName;
	CSpriteProduct* m_pTmpl;

	cocos2d::Sprite* m_pSprite;
	cocos2d::Vec4 m_v4Trans;
};