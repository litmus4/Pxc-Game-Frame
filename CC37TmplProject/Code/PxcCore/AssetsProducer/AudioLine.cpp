#include "AudioLine.h"
#include "AudioProduct.h"

CAudioLine::CAudioLine()
{
	m_bHaveBgm = true;
	m_bHaveSound = true;
	m_fBgmVolume = 1.0f;
	m_fSoundVolume = 1.0f;
}

CAudioLine::~CAudioLine()
{
	//
}

CAudioProduct* CAudioLine::Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn)
{
	CAudioProduct* pProduct = static_cast<CAudioProduct*>(FetchProduct(iID, eLoadType, bAsyn));
	if (pProduct)
	{
		m_setAudios.insert(pProduct);
		m_mapWaits.insert(std::make_pair(pProduct->GetName(), std::list<bool>()));
		pProduct->SetLine(this);
	}
	return pProduct;
}

void CAudioLine::Discard(CAudioProduct* pProduct)
{
	if (pProduct)
	{
		m_setAudios.erase(pProduct);
		m_mapWaits.erase(pProduct->GetName());
	}
	CBaseLine::Discard(pProduct);
}

void CAudioLine::Update(float dt)
{
	CBaseLine::Update(dt);

	std::map<int, ProductTmpl>::iterator iter = m_mapTemplates.begin();
	for (; iter != m_mapTemplates.end(); iter++)
	{
		CAudioProduct* pProduct = static_cast<CAudioProduct*>(iter->second.pProduct);
		pProduct->Update(dt);
	}
}

void CAudioLine::SetBgm(bool bHave, float fVolume)
{
	m_bHaveBgm = bHave;
	m_fBgmVolume = fVolume;

	std::set<CAudioProduct*>::iterator iter = m_setAudios.begin();
	for (; iter != m_setAudios.end(); iter++)
	{
		if ((*iter)->IsBgm())
		{
			(*iter)->SetMuteExternally(!bHave);
			(*iter)->SetVolumeExternally(fVolume);
		}
	}
}

void CAudioLine::SetSound(bool bHave, float fVolume)
{
	m_bHaveSound = bHave;
	m_fSoundVolume = fVolume;

	std::set<CAudioProduct*>::iterator iter = m_setAudios.begin();
	for (; iter != m_setAudios.end(); iter++)
	{
		if (!(*iter)->IsBgm())
		{
			(*iter)->SetMuteExternally(!bHave);
			(*iter)->SetVolumeExternally(fVolume);
		}
	}
}

bool CAudioLine::HaveBgm()
{
	return m_bHaveBgm;
}

bool CAudioLine::HaveSound()
{
	return m_bHaveSound;
}

float CAudioLine::GetBgmVolume()
{
	return m_fBgmVolume;
}

float CAudioLine::GetSoundVolume()
{
	return m_fSoundVolume;
}

void CAudioLine::PushWait(std::string& strName, bool bLoop)
{
	std::map<std::string, std::list<bool>>::iterator iter = m_mapWaits.find(strName);
	if (iter != m_mapWaits.end())
		iter->second.push_back(bLoop);
}

bool CAudioLine::PopWait(std::string& strName, bool& bLoopOut)
{
	std::map<std::string, std::list<bool>>::iterator iter = m_mapWaits.find(strName);
	if (iter != m_mapWaits.end())
	{
		if (iter->second.empty())
			return false;
		bLoopOut = iter->second.front();
		iter->second.pop_front();
		return true;
	}
	return false;
}

CBaseProduct* CAudioLine::CreateProduct()
{
	return new CAudioProduct();
}