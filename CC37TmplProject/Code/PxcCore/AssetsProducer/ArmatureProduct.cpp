#include "ArmatureProduct.h"
#include "SubThreadDataReader.h"

CArmatureProduct::CurrentAction::CurrentAction()
{
	//
}

void CArmatureProduct::CurrentAction::Reset()
{
	//
}

CArmatureProduct::PartAction::PartAction()
{
	//
}

void CArmatureProduct::PartAction::Reset()
{
	//
}

CArmatureProduct::CArmatureProduct()
{
	//
}

CArmatureProduct::~CArmatureProduct()
{
	//
}

std::string CArmatureProduct::GetConfigFileName()
{
	return m_strConfigFileName;
}

bool CArmatureProduct::Read(PxcUtil::CCSVTableOperator& tabop, GlobalDef::ELanguage eLanguage)
{
	return true;
}

bool CArmatureProduct::Load()
{
	return true;
}

void CArmatureProduct::UnLoad()
{
	//
}

CBaseProduct* CArmatureProduct::Clone()
{
	CArmatureProduct* pProduct = new CArmatureProduct();
	pProduct->m_iID = m_iID;
	pProduct->m_strConfigFileName = m_strConfigFileName;
	pProduct->m_strPlistFileName = m_strPlistFileName;
	pProduct->m_strImageFileName = m_strImageFileName;
	pProduct->m_strActionFileName = m_strActionFileName;
	pProduct->m_pArmature = m_pArmature;
	pProduct->m_pTmpl = this;
	return pProduct;
}

void CArmatureProduct::Update(float dt)
{
	//
}

void CArmatureProduct::Draw(cocos2d::Renderer* renderer, const cocos2d::Mat4& parentTrans, uint32_t parentFlags)
{
	//
}

bool CArmatureProduct::HaveSubThreadReader()
{
	return (m_pReader ? true : false);
}

cocostudio::Armature* CArmatureProduct::GetArmature()
{
	return m_pArmature;
}

void CArmatureProduct::SetTransform(cocos2d::Vec4& v4Trans, bool bForce)
{
	//
}

cocos2d::Vec4 CArmatureProduct::GetTransform()
{
	return m_v4Trans;
}

bool CArmatureProduct::PlayAction(int iActionID, CArmatureActionInfo::EParentBinderType ePrtBinderType,
						CArmatureActionInfo::EDynamicSkinType eDynSkinType, float fStartFrame,
						float fSpeedScale, bool bBlend, bool bReplay, int* pAttachedEffectID)
{
	return true;
}

void CArmatureProduct::StopAction(int iActionID, bool bLoopOnly, bool bPlayLeave)
{
	//
}

void CArmatureProduct::StopAction(EPriorityType ePriorityType, bool bPlayLeave)
{
	//
}

CArmatureProduct::tMainBinderMap* CArmatureProduct::GetMainActionMap()
{
	return m_pmapActions;
}

std::map<int, CArmatureProduct::ActionGroup>* CArmatureProduct::GetExcitedActionMap(
	CArmatureActionInfo::EParentBinderType ePrtBinderType, CArmatureActionInfo::EDynamicSkinType eDynSkinType)
{
	return NULL;
}

CArmatureActionInfo* CArmatureProduct::GetPlayingActionInfo(EPartType ePartType)
{
	return NULL;
}

void CArmatureProduct::OnLoadComplete()
{
	//
}

void CArmatureProduct::OnPartCompleteCallback(cocostudio::Armature* pArmature, const std::string& strBoneName,
						cocostudio::MovementEventType eEventType, const std::string& strAnimName)
{
	//
}

CArmatureActionInfo* CArmatureProduct::GetActionInfo(CArmatureActionInfo::EParentBinderType ePrtBinderType,
									CArmatureActionInfo::EDynamicSkinType eDynSkinType, int iActionID)
{
	return NULL;
}

int CArmatureProduct::SetCurrentAction(CArmatureActionInfo* pActionInfo, float fStartFrame, float fStartTime,
						float fSpeedScale, bool bBlend, bool bReplay)
{
	return 0;
}

int CArmatureProduct::SetPartAction(EPartType ePartType, EPriorityType ePriorityType)
{
	return 0;
}

void CArmatureProduct::RefreshAction()
{
	//
}