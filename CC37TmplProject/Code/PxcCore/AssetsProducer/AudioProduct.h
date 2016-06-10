#pragma once
#include "BaseProduct.h"

class CAudioLine;
class CAudioProduct : public CBaseProduct
{
public:
	struct Ident
	{
		std::string strName;
		int iAudioID;
	};

	struct SharedSoundItem
	{
		Ident ident;
		CAudioProduct* pLink;
		float fTime;
	};

public:
	CAudioProduct();
	~CAudioProduct();

	std::string GetName();
	bool IsBgm();
	int GetAudioID();

	virtual bool Read(PxcUtil::CCSVTableOperator& tabop);
	virtual bool Load();
	virtual void UnLoad();
	virtual CBaseProduct* Clone();

	void SetLine(CAudioLine* pLine);
	void Update(float dt);
	void Play(bool bLoop, bool bForceStopNow = true);
	void Stop();
	bool IsPlaying(bool bSelf = true);
	void SetVolume(float fVol);
	float GetVolume();
	void SetMuteExternally(bool b);
	void SetVolumeExternally(float fVol);

	virtual void OnLoadComplete();

	Ident& CreateSharedSound(CAudioProduct* pLink);
	Ident& ReloadRelation(Ident& ident, CAudioProduct* pLink);
	bool CheckAndCutLink(Ident& ident, CAudioProduct* pLink);

private:
	Ident& SelectBestSharedSound(bool pbMax, CAudioProduct* pRobLink);
	void OnPlayEnd(int iAudioID, const std::string& strName, bool bManual);

private:
	Ident m_Ident;
	bool m_bBgm;
	float m_fFirmVolume;//表中填写的固定音量
	CAudioProduct* m_pTmpl;
	CAudioLine* m_pLine;

	std::vector<SharedSoundItem> m_vecSharedSounds;

	bool m_bPlaying;
	bool m_bLoop;
	float m_fVolume;
	bool m_bMuteEx;
	bool m_fVolumeEx;
};