#include "AudioEngine.h"
#include "AudioProduct.h"
#include "PxcUtil/Scattered.h"

using namespace cocos2d::experimental;

CAudioProduct::CAudioProduct()
{
	m_Ident.iAudioID = -1;
	m_bBgm = false;
	m_fFirmVolume = 0.0f;
	m_pTmpl = NULL;

	m_bPlaying = false;
	m_bLoop = false;
	m_fVolume = 1.0f;
	m_bMuteEx = false;
	m_fVolumeEx = 1.0f;
}

CAudioProduct::~CAudioProduct()
{
	//
}

std::string CAudioProduct::GetName()
{
	return m_Ident.strName;
}

bool CAudioProduct::IsBgm()
{
	return m_bBgm;
}

int CAudioProduct::GetAudioID()
{
	return m_Ident.iAudioID;
}

bool CAudioProduct::Read(PxcUtil::CCSVTableOperator& tabop)
{
	if (!tabop.GetValue("ID", m_iID))
		return false;

	if (!tabop.GetValue("Name", m_Ident.strName))
		return false;

	if (!tabop.GetValue("IsBgm", m_bBgm))
		return false;

	if (!tabop.GetValue("Volume", m_fFirmVolume))
		return false;

	return true;
}

bool CAudioProduct::Load()
{
	if (IsLoaded())
		return true;

	if (m_eLoadType == CBaseProduct::ESharedData)
	{
		if (m_pTmpl)
		{
#ifdef AUDIO_MEMORY_REUSE
			m_Ident = m_pTmpl->CreateSharedSound(this);
#else
			m_Ident = m_pTmpl->m_Ident;
#endif
		}
	}
	else
	{
		AudioEngine::preload(m_Ident.strName, true);
#ifdef AUDIO_MEMORY_REUSE
		if (m_eLoadType == CBaseProduct::ESharedProduct)
		{
			SharedSoundItem item;
			item.ident = m_Ident;
			item.pLink = this;
			item.fTime = -1.0f;
			m_vecSharedSounds.push_back(item);
		}
#endif
	}

	CBaseProduct::Load();
	return true;
}

void CAudioProduct::UnLoad()
{
	if (!IsLoaded())
		return;

	if (!m_Ident.strName.empty() && m_eLoadType == CBaseProduct::EClonedData)
	{
		AudioEngine::stop(m_Ident.iAudioID);
		AudioEngine::uncache(m_Ident.strName);
		m_Ident.iAudioID = -1;
	}
	else if (m_eLoadType == CBaseProduct::ESharedProduct)
	{
#ifdef AUDIO_MEMORY_REUSE
		std::vector<SharedSoundItem>::iterator iter = m_vecSharedSounds.begin();
		for (; iter != m_vecSharedSounds.end(); iter++)
		{
			if (iter->ident.strName.empty())
				continue;
			AudioEngine::stop(iter->ident.iAudioID);
			AudioEngine::uncache(iter->ident.strName);
		}
		m_vecSharedSounds.clear();
#else
		AudioEngine::uncache(m_Ident.strName);
#endif
	}

	CBaseProduct::UnLoad();
}

CBaseProduct* CAudioProduct::Clone()
{
	CAudioProduct* pProduct = new CAudioProduct();
	pProduct->m_iID = m_iID;
	pProduct->m_Ident = m_Ident;
	pProduct->m_bBgm = m_bBgm;
	pProduct->m_fFirmVolume = m_fFirmVolume;
	pProduct->m_pTmpl = this;
	return pProduct;
}

void CAudioProduct::Update(float dt)
{
#ifdef AUDIO_MEMORY_REUSE
	if (IsComplete() && m_eLoadType == CBaseProduct::ESharedData)
	{
		std::vector<SharedSoundItem>::iterator iter = m_vecSharedSounds.begin();
		for (; iter != m_vecSharedSounds.end(); iter++)
		{
			if (iter->ident.strName.empty())
				continue;
			if (iter->fTime > -0.5f)
				iter->fTime += dt;
		}
	}
#endif
}

void CAudioProduct::Play(bool bLoop)
{
	if (IsComplete())
	{
#ifdef AUDIO_MEMORY_REUSE
		if (m_pTmpl && m_eLoadType == CBaseProduct::ESharedData)
			m_Ident = m_pTmpl->ReloadRelation(m_Ident, this);
#endif

		m_Ident.iAudioID = AudioEngine::play2d(m_Ident.strName, bLoop);
		SetVolume(m_fVolume);
		m_bPlaying = true;
	}
	else
	{
		m_bPlaying = true;
		m_bLoop = bLoop;
	}
}

void CAudioProduct::Stop()
{
	if (IsComplete())
	{
		bool bOk = true;
#ifdef AUDIO_MEMORY_REUSE
		if (m_pTmpl && m_eLoadType == CBaseProduct::ESharedData)
			bOk = m_pTmpl->CheckAndCutLink(m_Ident, this);
#endif
		if (bOk)
		{
			AudioEngine::stop(m_Ident.iAudioID);
			m_bPlaying = false;
		}
	}
	else
		m_bPlaying = false;
}

bool CAudioProduct::IsPlaying()
{
	if (IsComplete())
	{
		if (m_Ident.iAudioID == -1 || !m_bPlaying)
			return false;
		return (AudioEngine::getState(m_Ident.iAudioID) == AudioEngine::AudioState::PLAYING);
	}
	return m_bPlaying;
}

void CAudioProduct::SetVolume(float fVol)
{
	if (fVol < 0.0f) fVol = 0.0f;
	if (fVol > 1.0f) fVol = 1.0f;

	m_fVolume = fVol;
	if (IsComplete() && m_Ident.iAudioID != -1)
	{
		float fValue = fVol * m_fVolumeEx * m_fFirmVolume;
		AudioEngine::setVolume(m_Ident.iAudioID, fValue);
	}
}

float CAudioProduct::GetVolume()
{
	if (IsComplete())
		return AudioEngine::getVolume(m_Ident.iAudioID);
	return m_fVolume;
}

void CAudioProduct::SetMuteExternally(bool b)
{
	if (m_bMuteEx != b)
	{
		m_bMuteEx = b;
		if (m_bMuteEx && IsComplete() && m_Ident.iAudioID != 1 &&
			AudioEngine::getState(m_Ident.iAudioID) == AudioEngine::AudioState::PLAYING)
			Stop();
	}
}

void CAudioProduct::SetVolumeExternally(float fVol)
{
	if (fabs(m_fVolumeEx - fVol) >= FLOAT_PRECISION)
	{
		m_fVolumeEx = fVol;
		SetVolume(m_fVolume);
	}
}

void CAudioProduct::OnLoadComplete()
{
	CBaseProduct::OnLoadComplete();
	if (m_bPlaying)
		Play(m_bLoop);
}

CAudioProduct::Ident& CAudioProduct::CreateSharedSound(CAudioProduct* pLink)
{
	//WAIT
	return m_Ident;
}

CAudioProduct::Ident& CAudioProduct::ReloadRelation(Ident& ident, CAudioProduct* pLink)
{
	//WAIT
	return m_Ident;
}

bool CAudioProduct::CheckAndCutLink(Ident& ident, CAudioProduct* pLink)
{
	//WAIT
	return false;
}

CAudioProduct::Ident& CAudioProduct::SelectBestSharedSound(bool pbMax, CAudioProduct* pRobLink)
{
	//WAIT
	return m_Ident;
}