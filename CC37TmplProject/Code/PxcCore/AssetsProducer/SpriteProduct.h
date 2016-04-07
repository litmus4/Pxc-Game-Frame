#pragma once
#include "cocos2d.h"
#include "BaseProduct.h"

class CAnimateProduct;
class CSpriteLine;
class CSubThreadDataReader;
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

	void SetLine(CSpriteLine* pLine);
	void Update(float dt);
	void Draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags);
	bool HaveSubThreadReader();
	cocos2d::Sprite* GetSprite();
	void SetTransform(cocos2d::Vec4& v4Trans, bool bForce = false);
	cocos2d::Vec4 GetTransform();
	void AddAnimate(CAnimateProduct* pAnimPro);
	void RemoveAnimate(std::string strAnimName);
	CAnimateProduct* GetAnimate(std::string strAnimName);
	void BindAnimates();

	virtual void OnLoadComplete();
	virtual void OnBeforeUnLoad();

private:
	void CopyFromTemplate();

private:
	std::string m_strName;
	bool m_bSpriteFrame;
	std::string m_strPlistFileName;
	std::string m_strAtlasFileName;
	bool m_bLoadingDraw;
	CSpriteProduct* m_pTmpl;
	CSpriteLine* m_pLine;

	CSubThreadDataReader* m_pReader;
	cocos2d::Sprite* m_pSprite;
	cocos2d::Vec4 m_v4Trans;
	std::map<std::string, CAnimateProduct*> m_mapAnims;
};