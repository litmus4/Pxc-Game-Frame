#include "SpriteProduct.h"
#include "AnimateProduct.h"
#include "SpriteLine.h"
#include "SubThreadDataReader.h"

CSpriteProduct::CSpriteProduct()
{
	m_bSpriteFrame = false;
	m_bLoadingDraw = false;
	m_pTmpl = NULL;
	m_pLine = NULL;

	m_pReader = NULL;
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

	if (!tabop.GetValue("IsLoadingDraw", m_bLoadingDraw))
		return false;

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
		m_pReader = new CSubThreadDataReader();
		if (m_bSpriteFrame)
		{
			m_pReader->LoadTexImage(m_strAtlasFileName);
			m_pReader->LoadPlist(m_strPlistFileName);
			//cocos2d::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(m_strPlistFileName, m_strAtlasFileName);
			//m_pSprite = cocos2d::Sprite::createWithSpriteFrameName(m_strName);
		}
		else
			m_pReader->LoadTexImage(m_strName);
			//m_pSprite = cocos2d::Sprite::create(m_strName);
	}
	else if (m_pTmpl->GetSprite())
		CopyFromTemplate();
	else if (!m_pTmpl->HaveSubThreadReader())
		return false;

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
	pProduct->m_bLoadingDraw = m_bLoadingDraw;
	pProduct->m_pTmpl = this;
	return pProduct;
}

void CSpriteProduct::SetLine(CSpriteLine* pLine)
{
	m_pLine = pLine;
}

void CSpriteProduct::Update(float dt)
{
	//TODO
}

void CSpriteProduct::Draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags)
{
	if (!IsComplete())
	{
		if (m_bLoadingDraw && m_pLine)
			m_pLine->DrawLoading(m_v4Trans, renderer, parentTrans, parentFlags);
		return;
	}
	if (m_pSprite)
		m_pSprite->visit(renderer, parentTrans, parentFlags);
}

bool CSpriteProduct::HaveSubThreadReader()
{
	return (m_pReader ? true : false);
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

void CSpriteProduct::AddAnimate(CAnimateProduct* pAnimPro)
{
	if (pAnimPro)
		m_mapAnims.insert(std::make_pair(pAnimPro->GetName(), pAnimPro));
}

void CSpriteProduct::RemoveAnimate(std::string strAnimName)
{
	m_mapAnims.erase(strAnimName);
}

CAnimateProduct* CSpriteProduct::GetAnimate(std::string strAnimName)
{
	std::map<std::string, CAnimateProduct*>::iterator iter = m_mapAnims.find(strAnimName);
	if (iter != m_mapAnims.end())
		return iter->second;
	return NULL;
}

void CSpriteProduct::BindAnimates()
{
	std::map<std::string, CAnimateProduct*>::iterator iter = m_mapAnims.begin();
	for (; iter != m_mapAnims.end(); iter++)
		iter->second->SetSprite(this);
}

void CSpriteProduct::OnLoadComplete()
{
	if (m_pReader)
	{
		if (m_bSpriteFrame)
		{
			m_pReader->AddSpriteFrames();
			m_pSprite = cocos2d::Sprite::createWithSpriteFrameName(m_strName);
		}
		else
			m_pSprite = m_pReader->CreateSprite();
		if (m_pSprite)
			m_pSprite->retain();
		SAFE_DELETE(m_pReader)
	}
	else if (m_pTmpl && m_pTmpl->GetSprite())
		CopyFromTemplate();
	CBaseProduct::OnLoadComplete();

	std::map<std::string, CAnimateProduct*>::iterator iter = m_mapAnims.begin();
	for (; iter != m_mapAnims.end(); iter++)
		iter->second->SetSprite(this);
	if (fabs(m_v4Trans.w - 0.0f) < FLOAT_PRECISION && m_pSprite)
		m_v4Trans.w = 1.0f;
	SetTransform(m_v4Trans);
}

void CSpriteProduct::OnBeforeUnLoad()
{
	CBaseProduct::OnBeforeUnLoad();
	std::map<std::string, CAnimateProduct*>::iterator iter = m_mapAnims.begin();
	for (; iter != m_mapAnims.end(); iter++)
		iter->second->SetSprite(NULL, false);
	m_mapAnims.clear();
}

void CSpriteProduct::CopyFromTemplate()
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
	if (m_pSprite)
		m_pSprite->retain();
}