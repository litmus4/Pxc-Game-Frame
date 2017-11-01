#pragma once
#include <string>

class CArmatureActionInfo
{
public:
	enum EParentBinderType
	{
		EParentBinder_None = 0,
	};

	enum EDynamicSkinType
	{
		EDynamicSkin_None = 0,
	};

public:
	CArmatureActionInfo()
	{
		m_iID = -1;
		m_eParentBinderType = EParentBinder_None;
		m_eDynamicSkinType = EDynamicSkin_None;
		m_iActionID = -1;
		m_bLoop = false;
		m_bRandom = false;
		m_iPriorityType = 0;
		m_iAttachedEffectID = -1;
	}

	~CArmatureActionInfo(){}

	int m_iID;
	EParentBinderType m_eParentBinderType;
	EDynamicSkinType m_eDynamicSkinType;
	int m_iActionID;
	std::string m_strEnterAnimName;
	std::string m_strAnimName;
	std::string m_strLeaveAnimName;
	bool m_bLoop;
	bool m_bRandom;
	int m_iPriorityType;
	int m_iAttachedEffectID;
	std::string m_strDesc;
};