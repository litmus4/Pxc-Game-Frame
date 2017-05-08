#pragma once
#include "cocos2d.h"
#include "cocostudio\CocoStudio.h"
#include "BaseProduct.h"
#include "ArmatureActionInfo.h"
#include "PxcUtil\Delegate.h"

class CSubThreadDataReader;
class CArmatureProduct : public CBaseProduct
{
public:
	enum EPriorityType
	{
		EPriority_None = 0,//0,���°���������
		EPriority_Up,//1,�ϰ�������
		EPriority_Down,//2,�°�������
		EPriority_Whole,//3,���岥��(���ܷ����°���)
		EPriority_Temp,//4,��ʱ����
		EPriority_Num,
	};
	enum EPartType
	{
		EPart_Up = 0,//�ϰ���
		EPart_Down,//�°���
		EPart_Num,
	};

	struct CurrentAction
	{
		enum EState
		{
			ENoAction = -1,//δ��ʼ����
			EEnterAnimation = 0,//���ڲ������붯��
			EAnimation,//���ڲ��ű��ֶ���
			ELeaveAnimation,//���ڲ����г�����
			EEndAction,
		};
		CurrentAction();
		void Reset();

		EState eState;
		CArmatureActionInfo* pActionInfo;
		float fStartFrame;
		float fStartTime;
		float fSpeedScale;
		bool bBlend;
	};
	struct PartAction
	{
		PartAction();
		void Reset();

		int iPriority;
		CArmatureActionInfo* pActionInfo;
		std::string strBoneName;
		std::string strAnimName;
		float fStartFrame;
		float fSpeedScale;
		bool bLoop;
		bool bChanged;
	};

	struct RequestAction
	{
		int iActionID;
		CArmatureActionInfo::EParentBinderType eParentBinderType;
		CArmatureActionInfo::EDynamicSkinType eDynamicSkinType;
		float fStartFrame;
		float fStartTime;
		float fSpeedScale;
		bool bBlend;
	};

	struct ActionGroup
	{
		ActionGroup()
		{
			bRandom = true;
			iCurIndex = 0;
		}

		bool bRandom;
		int iCurIndex;
		std::vector<CArmatureActionInfo*> vecActions;
	};

public:
	CArmatureProduct();
	virtual ~CArmatureProduct();

protected:
	//

private:
	std::string m_strConfigFileName;
	std::string m_strPlistFileName;
	std::string m_strImageFileName;
	std::string m_strActionFileName;
	CArmatureProduct* m_pTmpl;

	CSubThreadDataReader* m_pReader;
	cocostudio::Armature* m_pArmature;
	typedef std::map<CArmatureActionInfo::EDynamicSkinType,
		std::map<int/*ActionID*/, ActionGroup>> tSubSkinMap;
	typedef std::map<CArmatureActionInfo::EParentBinderType, tSubSkinMap> tMainBinderMap;
	tMainBinderMap m_mapActions;
	float m_fAnimInternal;
	cocos2d::Vec4 m_v4Trans;

	std::list<RequestAction> m_lisRequestActions;
	CurrentAction m_CurrentActions[EPriority_Num];
	PartAction m_PartActions[EPart_Num];
	std::string m_strPartBoneNames[3];
};