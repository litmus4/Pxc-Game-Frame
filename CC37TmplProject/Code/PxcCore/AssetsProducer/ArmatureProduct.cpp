#include "ArmatureProduct.h"
#include "SubThreadDataReader.h"

CArmatureProduct::CurrentAction::CurrentAction()
{
	Reset();
}

void CArmatureProduct::CurrentAction::Reset()
{
	eState = ENoAction;
	pActionInfo = NULL;
	fStartFrame = 0.0f;
	fStartTime = 0.0f;
	fSpeedScale = 1.0f;
	bBlend = false;
}

CArmatureProduct::PartAction::PartAction()
{
	Reset();
}

void CArmatureProduct::PartAction::Reset()
{
	iPriority = -1;
	pActionInfo = NULL;
	strBoneName.clear();
	strAnimName.clear();
	fStartFrame = 0.0f;
	fSpeedScale = 1.0f;
	bLoop = false;
	bChanged = false;
}

CArmatureProduct::CArmatureProduct()
: m_pTmpl(NULL)
, m_pReader(NULL)
, m_pArmature(NULL)
, m_pmapActions(NULL)
{
	m_fAnimInternal = 1.0f / 60.0f;
}

CArmatureProduct::~CArmatureProduct()
{
	if (m_pTmpl || !m_pmapActions)
		return;

	tMainBinderMap::iterator itMain = m_pmapActions->begin();
	for (; itMain != m_pmapActions->end(); itMain++)
	{
		tSubSkinMap::iterator itSub = itMain->second.begin();
		for (; itSub != itMain->second.end(); itSub++)
		{
			std::map<int, ActionGroup>::iterator itExcited = itSub->second.begin();
			for (; itExcited != itSub->second.end(); itExcited++)
			{
				std::vector<CArmatureActionInfo*>::iterator itInfo = itExcited->second.vecActions.begin();
				for (; itInfo != itExcited->second.vecActions.end(); itInfo++)
					delete *itInfo;
			}
		}
	}
	m_pmapActions->clear();
	delete m_pmapActions;
	m_pmapActions = NULL;
}

std::string CArmatureProduct::GetConfigFileName()
{
	return m_strConfigFileName;
}

bool CArmatureProduct::Read(PxcUtil::CCSVTableOperator& tabop, GlobalDef::ELanguage eLanguage)
{
	if (!tabop.GetValue("ID", m_iID))
		return false;

	if (!tabop.GetValue("FileName", m_strName))
		return false;

	m_strConfigFileName = m_strName + ".ExportJson";
	m_strPlistFileName = m_strName + ".plist";
	m_strImageFileName = m_strName + ".png";
	m_strActionFileName = m_strName + ".acts";
	return true;
}

bool CArmatureProduct::Load()
{
	if (IsLoaded())
		return true;
	if (m_eLoadType == ESharedData)
		return false;

	if (m_pTmpl == NULL)
	{
		m_pReader = new CSubThreadDataReader();
		m_pReader->LoadTexImage(m_strImageFileName);
		m_pReader->LoadPlist(m_strPlistFileName);
		m_pReader->LoadArmConfig(m_strConfigFileName);

		PxcUtil::CCSVTableOperator tabop;
		if (!m_pmapActions && tabop.Load(m_strActionFileName.c_str()))
		{
			m_pmapActions = new tMainBinderMap();
			while (tabop.ReadRow())
			{
				CArmatureActionInfo* pInfo = new CArmatureActionInfo();
				tabop.GetValue("ID", pInfo->m_iID);
				int iType = 0;
				tabop.GetValue("ParentBinderType", iType);
				pInfo->m_eParentBinderType = (CArmatureActionInfo::EParentBinderType)iType;
				tabop.GetValue("DynamicSkinType", iType);
				pInfo->m_eDynamicSkinType = (CArmatureActionInfo::EDynamicSkinType)iType;
				tabop.GetValue("ActionID", pInfo->m_iActionID);
				tabop.GetValue("EnterAnimName", pInfo->m_strEnterAnimName);
				tabop.GetValue("AnimName", pInfo->m_strAnimName);
				tabop.GetValue("LeaveAnimName", pInfo->m_strLeaveAnimName);
				tabop.GetValue("Loop", pInfo->m_bLoop);
				tabop.GetValue("Random", pInfo->m_bRandom);
				tabop.GetValue("PriorityType", pInfo->m_iPriorityType);
				tabop.GetValue("AttachedEffectID", pInfo->m_iAttachedEffectID);
				tabop.GetValue("Desc", pInfo->m_strDesc);

				tMainBinderMap::iterator itMain = m_pmapActions->find(pInfo->m_eParentBinderType);
				if (itMain == m_pmapActions->end())
				{
					std::pair<tMainBinderMap::iterator, bool> ret =
						m_pmapActions->insert(std::make_pair(pInfo->m_eParentBinderType, tSubSkinMap()));
					itMain = ret.first;
				}
				tSubSkinMap::iterator itSub = itMain->second.find(pInfo->m_eDynamicSkinType);
				if (itSub == itMain->second.end())
				{
					std::pair<tSubSkinMap::iterator, bool> ret =
						itMain->second.insert(std::make_pair(pInfo->m_eDynamicSkinType, std::map<int, ActionGroup>()));
					itSub = ret.first;
				}
				std::map<int, ActionGroup>& mapExcited = itSub->second;
				std::map<int, ActionGroup>::iterator itExcited = mapExcited.find(pInfo->m_iActionID);
				if (itExcited == mapExcited.end())
				{
					ActionGroup group;
					group.bRandom = pInfo->m_bRandom;
					std::pair<std::map<int, ActionGroup>::iterator, bool> ret =
						mapExcited.insert(std::make_pair(pInfo->m_iActionID, group));
					itExcited = ret.first;
				}
				itExcited->second.vecActions.push_back(pInfo);
			}
			tabop.Reset();
		}
	}
	else if (m_pTmpl->GetArmature())
	{
		m_pArmature = cocostudio::Armature::create(m_strName);
		m_pmapActions = m_pTmpl->m_pmapActions;
	}
	else if (!m_pTmpl->HaveSubThreadReader())
		return false;

	CBaseProduct::Load();
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