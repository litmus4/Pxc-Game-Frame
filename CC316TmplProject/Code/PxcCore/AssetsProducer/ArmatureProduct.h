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
		EPriority_None = 0,//0,上下半身都不优先
		EPriority_Up,//1,上半身优先
		EPriority_Down,//2,下半身优先
		EPriority_Whole,//3,整体播放(不能分上下半身)
		EPriority_Temp,//4,临时播放
		EPriority_Num,
	};
	enum EPartType
	{
		EPart_Up = 0,//上半身
		EPart_Down,//下半身
		EPart_Num,
	};

	struct CurrentAction
	{
		enum EState
		{
			ENoAction = -1,//未开始动作
			EEnterAnimation = 0,//正在播放切入动画
			EAnimation,//正在播放保持动画
			ELeaveAnimation,//正在播放切出动画
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
		CArmatureActionInfo::EParentBinderType ePrtBinderType;
		CArmatureActionInfo::EDynamicSkinType eDynSkinType;
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
	typedef std::map<CArmatureActionInfo::EDynamicSkinType,
		std::map<int/*ActionID*/, ActionGroup>> tSubSkinMap;
	typedef std::map<CArmatureActionInfo::EParentBinderType, tSubSkinMap> tMainBinderMap;

public:
	CArmatureProduct();
	virtual ~CArmatureProduct();

	std::string GetConfigFileName();
	virtual bool Read(PxcUtil::CCSVTableOperator& tabop, GlobalDef::ELanguage eLanguage);
	virtual bool Load();
	virtual void UnLoad();
	virtual CBaseProduct* Clone();

	void Update(float dt);
	void Draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags);
	bool HaveSubThreadReader();
	cocostudio::Armature* GetArmature();
	void SetTransform(cocos2d::Vec4& v4Trans, bool bForce = false);
	cocos2d::Vec4 GetTransform();

	bool PlayAction(int iActionID, CArmatureActionInfo::EParentBinderType ePrtBinderType,
		CArmatureActionInfo::EDynamicSkinType eDynSkinType,
		float fStartFrame = 0.0f, float fSpeedScale = 1.0f, bool bBlend = true,
		bool bReplay = true, int* pAttachedEffectID = NULL);
	void StopAction(int iActionID, bool bLoopOnly = true, bool bPlayLeave = true);
	void StopAction(EPriorityType ePriorityType, bool bPlayLeave = true);

	tMainBinderMap* GetMainActionMap();
	std::map<int/*ActionID*/, ActionGroup>*
		GetExcitedActionMap(CArmatureActionInfo::EParentBinderType ePrtBinderType,
		CArmatureActionInfo::EDynamicSkinType eDynSkinType);
	CArmatureActionInfo* GetPlayingActionInfo(EPartType ePartType);

	//TODOJK matrix

	virtual void OnLoadComplete();

protected:
	void OnPartCompleteCallback(cocostudio::Armature* pArmature, const std::string& strBoneName,
		cocostudio::MovementEventType eEventType, const std::string& strAnimName);
	CArmatureActionInfo* GetActionInfo(CArmatureActionInfo::EParentBinderType ePrtBinderType,
		CArmatureActionInfo::EDynamicSkinType eDynSkinType, int iActionID);
	int SetCurrentAction(CArmatureActionInfo* pActionInfo, float fStartFrame, float fStartTime,
		float fSpeedScale, bool bBlend, bool bReplay);
	int SetPartAction(EPartType ePartType, EPriorityType ePriorityType);
	void RefreshAction();

private:
	std::string m_strName;
	std::string m_strConfigFileName;
	std::string m_strPlistFileName;
	std::string m_strImageFileName;
	std::string m_strActionFileName;
	CArmatureProduct* m_pTmpl;

	CSubThreadDataReader* m_pReader;
	cocostudio::Armature* m_pArmature;
	tMainBinderMap* m_pmapActions;
	float m_fAnimInternal;
	cocos2d::Vec4 m_v4Trans;

	CurrentAction m_CurrentActions[EPriority_Num];
	PartAction m_PartActions[EPart_Num];
	std::list<RequestAction> m_lisRequestActions;
	std::string m_strPartBoneNames[3];
};