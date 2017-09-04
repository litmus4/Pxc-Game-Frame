#pragma once
#include "cocos2d.h"
#include "BaseProduct.h"

class CSpriteProduct;
class CSubThreadDataReader;
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

	virtual bool Read(PxcUtil::CCSVTableOperator& tabop, GlobalDef::ELanguage eLanguage);
	virtual bool Load();
	virtual void UnLoad();
	virtual CBaseProduct* Clone();

	void Update(float dt);
	bool HaveSubThreadReader();
	cocos2d::Animation* GetAnimation();
	void SetSprite(CSpriteProduct* pSpritePro, bool bNullRemove = true);
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
	std::string m_strSFPlistFile;
	std::string m_strSFAtlasFile;
	CAnimateProduct* m_pTmpl;

	CSubThreadDataReader* m_pReader;
	cocos2d::Animation* m_pAnimation;
	float m_fFps;
	bool m_bRestoredFinally;
	CSpriteProduct* m_pSpritePro;
	cocos2d::Sprite* m_pSprite;
	cocos2d::Animate* m_pPlayingAnimate;
	cocos2d::Sequence* m_pPlayingSeq;
	bool m_bRequestWait;
};