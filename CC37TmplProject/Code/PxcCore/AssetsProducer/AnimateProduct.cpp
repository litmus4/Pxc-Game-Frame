#include "AnimateProduct.h"
#include "SpriteProduct.h"
#include "SubThreadDataReader.h"
#include "PxcUtil/Scattered.h"

CAnimateProduct::CAnimateProduct()
{
	m_pTmpl = NULL;

	m_pReader = NULL;
	m_pAnimation = NULL;
	m_fFps = -1.0f;
	m_bRestoredFinally = true;
	m_pSpritePro = NULL;
	m_pSprite = NULL;
	m_pPlayingAnimate = NULL;
	m_pPlayingSeq = NULL;
	m_bRequestWait = false;
}

CAnimateProduct::~CAnimateProduct()
{
	//
}

std::string CAnimateProduct::GetName()
{
	return m_strName;
}

float CAnimateProduct::GetFps()
{
	return m_fFps;
}

void CAnimateProduct::SetFps(float fFps)
{
	m_fFps = fFps;
	if (IsComplete() && m_pAnimation)
		m_pAnimation->setDelayPerUnit(1.0f / fFps);
}

bool CAnimateProduct::IsRestoredFinally()
{
	return m_bRestoredFinally;
}

void CAnimateProduct::SetRestoredFinally(bool b)
{
	m_bRestoredFinally = b;
	if (m_pAnimation)
		m_pAnimation->setRestoreOriginalFrame(b);
}

bool CAnimateProduct::Read(PxcUtil::CCSVTableOperator& tabop, GlobalDef::ELanguage eLanguage)
{
	if (!tabop.GetValue("ID", m_iID))
		return false;

	if (!tabop.GetValue("PlistFileName", m_strPlistFileName))
		return false;

	if (!tabop.GetValue("Name", m_strName))
		return false;

	if (!tabop.GetValue("SFPlistFile", m_strSFPlistFile))
		return false;
	
	bool bGlobal = false;
	if (!tabop.GetValue("IsGlobal", bGlobal))
		return false;
	if (!tabop.GetValue("SFAtlasFile", m_strSFAtlasFile))
		return false;
	else if (bGlobal)
		GlobalDef::InsertLanguageFolder(m_strSFAtlasFile, eLanguage);

	return true;
}

bool CAnimateProduct::Load()
{
	if (IsLoaded())
		return true;
	if (m_eLoadType == ESharedData)
		return false;

	if (m_pTmpl == NULL)
	{
		m_pReader = new CSubThreadDataReader();
		m_pReader->LoadAnimPlist(m_strPlistFileName);
		m_pReader->LoadTexImage(m_strSFAtlasFile);
		m_pReader->LoadPlist(m_strSFPlistFile);
		//cocos2d::AnimationCache::getInstance()->addAnimationsWithFile(m_strPlistFileName);
		//m_pAnimation = cocos2d::AnimationCache::getInstance()->getAnimation(m_strName);
	}
	else if (m_pTmpl->GetAnimation())
		m_pAnimation = m_pTmpl->GetAnimation()->clone();
	else if (!m_pTmpl->HaveSubThreadReader())
		return false;

	CBaseProduct::Load();
	return true;
}

void CAnimateProduct::UnLoad()
{
	if (IsLoaded())
	{
		if (m_pAnimation)
			m_pAnimation->release();
		CBaseProduct::UnLoad();
	}
}

CBaseProduct* CAnimateProduct::Clone()
{
	CAnimateProduct* pProduct = new CAnimateProduct();
	pProduct->m_iID = m_iID;
	pProduct->m_strName = m_strName;
	pProduct->m_strPlistFileName = m_strPlistFileName;
	pProduct->m_strSFPlistFile = m_strSFPlistFile;
	pProduct->m_strSFAtlasFile = m_strSFAtlasFile;
	pProduct->m_pTmpl = this;
	return pProduct;
}

void CAnimateProduct::Update(float dt)
{
	//TODOJK
}

bool CAnimateProduct::HaveSubThreadReader()
{
	return (m_pReader ? true : false);
}

cocos2d::Animation* CAnimateProduct::GetAnimation()
{
	return m_pAnimation;
}

void CAnimateProduct::SetSprite(CSpriteProduct* pSpritePro, bool bNullRemove)
{
	if (pSpritePro)
	{
		m_pSpritePro = pSpritePro;
		if (IsComplete() && m_pSpritePro->IsComplete())
			m_pSprite = m_pSpritePro->GetSprite();
		m_pSpritePro->AddAnimate(this);
	}
	else
	{
		m_pSprite = NULL;
		if (m_pSpritePro && bNullRemove)
			m_pSpritePro->RemoveAnimate(GetName());
		m_pSpritePro = NULL;
	}
}

CSpriteProduct* CAnimateProduct::GetSprite()
{
	return m_pSpritePro;
}

short CAnimateProduct::Play()
{
	if (!IsComplete())
	{
		m_bRequestWait = true;
		return 0;
	}
	else if (m_pAnimation && m_pSprite)
	{
		cocos2d::Vector<cocos2d::FiniteTimeAction*> vecActs;

		m_pPlayingAnimate = cocos2d::Animate::create(m_pAnimation);
		vecActs.pushBack(m_pPlayingAnimate);

		cocos2d::CallFunc* pCallAct = cocos2d::CallFunc::create(CC_CALLBACK_0(CAnimateProduct::OnPlayEnd, this, false));
		vecActs.pushBack(pCallAct);

		m_pPlayingSeq = cocos2d::Sequence::create(vecActs);
		m_pSprite->runAction(m_pPlayingSeq);
		return 1;
	}
	return -1;
}

void CAnimateProduct::Stop()
{
	if (!IsComplete())
		m_bRequestWait = false;
	else if (m_pPlayingAnimate && m_pSprite)
	{
		m_pSprite->stopAction(m_pPlayingSeq);
		OnPlayEnd(true);
	}
}

bool CAnimateProduct::IsPlaying()
{
	return (m_pPlayingAnimate != NULL);
}

cocos2d::Animate* CAnimateProduct::GetPlayingAnimate()
{
	return m_pPlayingAnimate;
}

void CAnimateProduct::OnLoadComplete()
{
	if (m_pReader)
	{
		//这里一定要先添加精灵帧，然后再添加动画
		m_pReader->AddSpriteFrames();
		m_pReader->AddAnimations();
		m_pAnimation = cocos2d::AnimationCache::getInstance()->getAnimation(m_strName);
		if (m_pAnimation)
			m_pAnimation->retain();
		SAFE_DELETE(m_pReader)
	}
	else if (m_pTmpl && m_pTmpl->GetAnimation())
		m_pAnimation = m_pTmpl->GetAnimation()->clone();
	CBaseProduct::OnLoadComplete();

	if (m_fFps < 0.0f && m_pAnimation)
		m_fFps = 1.0f / m_pAnimation->getDelayPerUnit();
	else
		SetFps(m_fFps);
	SetRestoredFinally(m_bRestoredFinally);

	if (m_pSpritePro && m_pSpritePro->IsComplete())
		m_pSprite = m_pSpritePro->GetSprite();
	if (m_bRequestWait)
		Play();
}

void CAnimateProduct::OnPlayEnd(bool bManual)
{
	m_pPlayingAnimate = NULL;
	m_pPlayingSeq = NULL;

	//TODOJK
}