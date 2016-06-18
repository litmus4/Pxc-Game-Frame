#pragma once
#include "BaseLine.h"

class CAudioProduct;
class CAudioLine : public CBaseLine
{
public:
	CAudioLine();
	virtual ~CAudioLine();

	CAudioProduct* Fetch(int iID, CBaseProduct::ELoadType eLoadType, bool bAsyn = false);
	virtual void Discard(CAudioProduct* pProduct);
	virtual void Update(float dt);

	void SetBgm(bool bHave, float fVolume);
	void SetSound(bool bHave, float fVolume);

	bool HaveBgm();
	bool HaveSound();
	float GetBgmVolume();
	float GetSoundVolume();

	void PushWait(std::string& strName, bool bLoop);
	bool PopWait(std::string& strName, bool& bLoopOut);
	void ClearWait(std::string& strName);

protected:
	virtual CBaseProduct* CreateProduct();

private:
	std::set<CAudioProduct*> m_setAudios;
	std::map<std::string, std::list<bool>> m_mapWaits;
	bool m_bHaveBgm;
	bool m_bHaveSound;
	float m_fBgmVolume;
	float m_fSoundVolume;
};