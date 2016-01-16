#include "SpriteProduct.h"

CSpriteProduct::CSpriteProduct()
{
	m_bSpriteFrame = false;
	m_pTmpl = NULL;

	m_pSprite = NULL;
}

CSpriteProduct::~CSpriteProduct()
{
	//
}

bool CSpriteProduct::IsSpriteFrame()
{
	return m_bSpriteFrame;
}

std::string CSpriteProduct::GetName()
{
	return m_strName;
}

bool CSpriteProduct::Read(PxcUtil::CCSVTableOperator& tabop)
{
	if (!tabop.GetValue("ID", m_iID))
		return false;

	if (!tabop.GetValue("IsSpriteFrame", m_bSpriteFrame))
		return false;

	if (m_bSpriteFrame)
	{
		if (!tabop.GetValue("PlistFileName", m_strPlistFileName))
			return false;

		if (!tabop.GetValue("TexFileName", m_strAtlasFileName))
			return false;

		if (!tabop.GetValue("FrameName", m_strName))
			return false;
	}
	else
	{
		if (!tabop.GetValue("TexFileName", m_strName))
			return false;
	}

	return true;
}

bool CSpriteProduct::Load()
{
	if (IsLoaded())
		return true;
	if (m_eLoadType == ESharedData)
		return false;

	if (m_pTmpl == NULL)
	{
		if (m_bSpriteFrame)
		{
			cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(m_strPlistFileName, m_strAtlasFileName);
			m_pSprite = cocos2d::Sprite::createWithSpriteFrameName(m_strName);
		}
		else
			m_pSprite = cocos2d::Sprite::create(m_strName);
	}
	else if (m_pTmpl->GetSprite())
	{
		if (m_bSpriteFrame)
		{
			cocos2d::SpriteFrame* pFrame = m_pTmpl->GetSprite()->getSpriteFrame();
			m_pSprite = cocos2d::Sprite::createWithSpriteFrame(pFrame);
		}
		else
		{
			cocos2d::Texture2D* pTex = m_pTmpl->GetSprite()->getTexture();
			m_pSprite = cocos2d::Sprite::createWithTexture(pTex);
		}
	}
	else
		return false;
	if (m_pSprite)
		m_pSprite->retain();

	CBaseProduct::Load();
	return true;
}

void CSpriteProduct::UnLoad()
{
	if (IsLoaded())
	{
		if (m_pSprite)
			m_pSprite->release();
		CBaseProduct::UnLoad();
	}
}

CBaseProduct* CSpriteProduct::Clone()
{
	CSpriteProduct* pProduct = new CSpriteProduct();
	pProduct->m_iID = m_iID;
	pProduct->m_strName = m_strName;
	pProduct->m_bSpriteFrame = m_bSpriteFrame;
	pProduct->m_strPlistFileName = m_strPlistFileName;
	pProduct->m_strAtlasFileName = m_strAtlasFileName;
	pProduct->m_pTmpl = this;
	return pProduct;
}

void CSpriteProduct::Update(float dt)
{
	//TODO
}

void CSpriteProduct::Draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags)
{
	if (!IsComplete())
		return;
	if (m_pSprite)
		m_pSprite->visit(renderer, parentTrans, parentFlags);
}

cocos2d::Sprite* CSpriteProduct::GetSprite()
{
	return m_pSprite;
}

void CSpriteProduct::SetTransform(cocos2d::Vec4& v4Trans, bool bForce)
{
	m_v4Trans = v4Trans;
	if (IsComplete() && m_pSprite)
	{
		if (!bForce && m_pSprite->getNumberOfRunningActions() > 0)
			return;

		m_pSprite->setPosition(cocos2d::Vec2(v4Trans.x, v4Trans.y));
		m_pSprite->setRotation(v4Trans.z);
		m_pSprite->setScale(v4Trans.w);
	}
}

cocos2d::Vec4 CSpriteProduct::GetTransform()
{
	return m_v4Trans;
}

void CSpriteProduct::OnLoadComplete()
{
	CBaseProduct::OnLoadComplete();
	SetTransform(m_v4Trans);
}