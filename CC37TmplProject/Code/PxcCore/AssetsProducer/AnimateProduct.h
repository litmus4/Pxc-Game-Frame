#pragma once
#include "cocos2d.h"
#include "BaseProduct.h"

class CSpriteProduct;
class CAnimateProduct : public CBaseProduct
{
public:
	CAnimateProduct();
	virtual ~CAnimateProduct();

	std::string GetName();
	float GetFps();
	void SetFps(float fFps);
	bool IsRestoredFinally();
	void SetRestoredFinally(bool b);

	virtual bool Read(PxcUtil::CCSVTableOperator& tabop);
	virtual bool Load();
	virtual void UnLoad();
	virtual CBaseProduct* Clone();

	void Update(float dt);
	cocos2d::Animation* GetAnimation();
	void SetSprite(CSpriteProduct* pSpritePro);
	CSpriteProduct* GetSprite();
	short Play();
	void Stop();
	bool IsPlaying();
	cocos2d::Animate* GetPlayingAnimate();

	virtual void OnLoadComplete();

protected:
	void OnPlayEnd(bool bManual);

private:
	std::string m_strName;
	std::string m_strPlistFileName;
	CAnimateProduct* m_pTmpl;

	cocos2d::Animation* m_pAnimation;
	CSpriteProduct* m_pSpritePro;
	cocos2d::Sprite* m_pSprite;
	cocos2d::Animate* m_pPlayingAnimate;
	cocos2d::Sequence* m_pPlayingSeq;
	bool m_bRequestWait;
};