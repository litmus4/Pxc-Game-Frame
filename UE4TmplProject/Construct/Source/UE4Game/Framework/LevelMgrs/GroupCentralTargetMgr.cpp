// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/GroupCentralTargetMgr.h"
#include "../PxcGameMode.h"

FGroupCentralData::FGroupCentralData()
	: fRecenterPrecision(0.0f), fFollowPrecision(0.0f)
	, fFollowSpeed(0.0f), fFollowAccTime(0.0f), fFollowDecTime(0.0f)
	, fDefaultMoveTime(-1.0f), pDefaultDynamicMover(nullptr)
	, fDefaultBlendTime(-1.0f), pCentralViewTarget(nullptr)
	, eDefaultBlendFunc(EViewTargetBlendFunction::VTBlend_Linear)
	, bFollowing(false), bFollowSpeed(false), fAcceleration(0.0f), fDeceleration(0.0f)
	, bMoving(false), pCurDirect(nullptr), pLastDirect(nullptr)
	, fCurMoveTime(0.0f), fDynamicMoveMax(0.0f), pCurView(nullptr)
{
	vCentralTarget = FVector(0.0f, 0.0f, 0.0f);
	vFollowTarget = FVector(0.0f, 0.0f, 0.0f);
	vFollowVelocity = FVector(0.0f, 0.0f, 0.0f);
	vDirectTarget = FVector(0.0f, 0.0f, 0.0f);
}

void FGroupCentralData::Init(const FName& xGroupName, float xRecenterPrecision, float xFollowPrecision,
	float xFollowSpeed, float xFollowAccTime, float xFollowDecTime)
{
	GroupName = xGroupName;
	fRecenterPrecision = FMath::Max(xRecenterPrecision, 0.0f);
	fFollowPrecision = FMath::Max(xFollowPrecision, 0.0f);
	fFollowSpeed = FMath::Max(xFollowSpeed, 0.0f);
	fFollowAccTime = FMath::Max(xFollowAccTime, 0.0f);
	fFollowDecTime = FMath::Max(xFollowDecTime, 0.0f);

	//a=v/t
	fAcceleration = fFollowSpeed / fFollowAccTime;
	fDeceleration = fFollowSpeed / fFollowDecTime;
}

void FGroupCentralData::Init(FVirtualGroup* pGroup, float xRecenterPrecision, float xFollowPrecision,
	float xFollowSpeed, float xFollowAccTime, float xFollowDecTime)
{
	check(pGroup);
	GroupName = pGroup->Name;
	fRecenterPrecision = FMath::Max(xRecenterPrecision, 0.0f);
	fFollowPrecision = FMath::Max(xFollowPrecision, 0.0f);
	fFollowSpeed = FMath::Max(xFollowSpeed, 0.0f);
	fFollowAccTime = FMath::Max(xFollowAccTime, 0.0f);
	fFollowDecTime = FMath::Max(xFollowDecTime, 0.0f);

	tsetBackActors = pGroup->tsetActors;
	//a=v/t
	fAcceleration = fFollowSpeed / fFollowAccTime;
	fDeceleration = fFollowSpeed / fFollowDecTime;
}

void FGroupCentralData::RefreshFollowState(bool bInit)
{
	if (bInit)
	{
		vFollowTarget = vCentralTarget;
		return;
	}

	if (!bFollowing)
	{
		if (FVector::Distance(vCentralTarget, vFollowTarget) >= fFollowPrecision)
		{
			bFollowing = true;
			bFollowSpeed = false;
		}
		else
			return;
	}

	FVector vDir = (vCentralTarget - vFollowTarget).GetSafeNormal();
	if (vDir.IsZero()) return;
	vFollowVelocity = (bFollowSpeed ? vDir * vFollowVelocity.Size() : vDir);
}

void FGroupCentralData::UpdateFollow(float fDeltaSeconds)
{
	//
}

void FGroupCentralData::SetDirect(float fMoveTime, UCurveFloat* pDynamicMover)
{
	fDefaultMoveTime = FMath::Max(fMoveTime, 0.0f);
	pDefaultDynamicMover = pDynamicMover;
	if (IsValid(pDynamicMover))
	{
		float fTempMin = 0.0f;
		pDynamicMover->FloatCurve.GetTimeRange(fTempMin, fDynamicMoveMax);
	}

	bMoving = true;
	pCurDirect = nullptr;//默认指向中心
	pLastDirect = nullptr;
	vDirectTarget = vFollowTarget;
}

void FGroupCentralData::SetView(float fBlendTime, EViewTargetBlendFunction eBlendFunc, AActor* pCentralVT)
{
	verify(IsValid(pCentralVT));
	fDefaultBlendTime = FMath::Max(fBlendTime, 0.0f);
	eDefaultBlendFunc = eBlendFunc;
	pCentralViewTarget = pCentralVT;

	pCurView = nullptr;//默认看中心
}

void FGroupCentralData::AddActorDirectInfo(AActor* pActor, float fMoveTime, UCurveFloat* pDynamicMover)
{
	check(IsValid(pActor));

	FGrpCtrActorDirectInfo Info;
	Info.fMoveTime = FMath::Max(fMoveTime, 0.0f);
	Info.pDynamicMover = pDynamicMover;
	if (IsValid(pDynamicMover))
	{
		float fTempMin = 0.0f;
		pDynamicMover->FloatCurve.GetTimeRange(fTempMin, Info.fDynamicMoveTime);
	}
	tmapActorDirectInfos.Add(pActor, Info);
}

void FGroupCentralData::AddActorViewInfo(AActor* pActor, AActor* pViewTarget, float fBlendTime, EViewTargetBlendFunction eBlendFunc)
{
	check(IsValid(pActor));
	verify(IsValid(pViewTarget));

	FGrpCtrActorViewInfo Info;
	Info.pViewTarget = pViewTarget;
	Info.fBlendTime = FMath::Max(fBlendTime, 0.0f);
	Info.eBlendFunc = eBlendFunc;
	tmapActorViewInfos.Add(pActor, Info);
}

void FGroupCentralData::ResetFloatings()
{
	fDefaultMoveTime = -1.0f;
	pDefaultDynamicMover = nullptr;
	tmapActorDirectInfos.Empty();

	fDefaultBlendTime = -1.0f;
	eDefaultBlendFunc = EViewTargetBlendFunction::VTBlend_Linear;
	pCentralViewTarget = nullptr;
	tmapActorViewInfos.Empty();
}

void UGroupCentralTargetMgr::SetCentralTarget(const FName& GroupName, float fRecenterPrecision, float fFollowPrecision,
	float fFollowSpeed, float fFollowAccTime, float fFollowDecTime)
{
	UVirtualGroupMgr* pManager = CastChecked<APxcGameMode>(GetOuter())->GetVirtualGroupMgr();
	check(pManager);
	FVirtualGroup* pGroup = pManager->GetGroup(GroupName);
	if (!pGroup || !pGroup->GetFeatureByUsage<FVirtGrpCentralFeature>(EVirtualGroupUsage::CentralTarget))
		return;

	if (m_tmapCentralDatas.Contains(GroupName))
		return;

	FGroupCentralData Data;
	Data.Init(pGroup, fRecenterPrecision, fFollowPrecision, fFollowSpeed, fFollowAccTime, fFollowDecTime);
	m_tmapCentralDatas.Add(GroupName, Data);

	for (AActor* pActor : pGroup->tsetActors)
	{
		USceneComponent* pComp = pActor->GetRootComponent();
		SLocationHelper Helper;
		Helper.vLastLocation = pActor->GetActorLocation();
		Helper.GroupName = GroupName;
		m_mapLocationHelpers.insert(std::make_pair(pComp, Helper));
		pComp->TransformUpdated.AddUObject(this, &UGroupCentralTargetMgr::OnActorTransformUpdated);
	}

	UpdateCentralTarget(GroupName, pManager);
}

void UGroupCentralTargetMgr::UpdateCentralTarget(const FName& GroupName, UVirtualGroupMgr* pManager, TSet<AActor*>* ptsetActors)
{
	bool bActorUpdated = (bool)ptsetActors;
	bool bInit = (bool)pManager;
	if (!bActorUpdated)
	{
		if (!bInit)
		{
			pManager = CastChecked<APxcGameMode>(GetOuter())->GetVirtualGroupMgr();
			check(pManager);
		}
		//if (!pManager->GetFeatureFromGroup<FVirtGrpCentralFeature>(EVirtualGroupUsage::CentralTarget, GroupName))
			//return;//功能内部调用此函数免检查
		FVirtualGroup* pGroup = pManager->GetGroup(GroupName);
		if (pGroup)
			ptsetActors = &pGroup->tsetActors;
	}

	if (ptsetActors && ptsetActors->Num() > 0)
	{
		FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
		if (pData)
		{
			TSet<AActor*> tsetTempActors;
			if (bActorUpdated)
			{
				tsetTempActors = pData->tsetBackActors;
				pData->tsetBackActors.Empty();
			}

			FVector vCentral(0.0f, 0.0f, 0.0f);
			for (AActor* pActor : *ptsetActors)
			{
				vCentral += pActor->GetActorLocation();

				if (bActorUpdated)
				{
					pData->tsetBackActors.Add(pActor);
					USceneComponent* pComp = pActor->GetRootComponent();
					std::unordered_map<USceneComponent*, SLocationHelper>::iterator iter = m_mapLocationHelpers.find(pComp);
					if (iter == m_mapLocationHelpers.end())
					{
						SLocationHelper Helper;
						Helper.vLastLocation = pActor->GetActorLocation();
						Helper.GroupName = GroupName;
						m_mapLocationHelpers.insert(std::make_pair(pComp, Helper));
						pComp->TransformUpdated.AddUObject(this, &UGroupCentralTargetMgr::OnActorTransformUpdated);
					}
					else
						tsetTempActors.Remove(pActor);
				}
			}
			pData->vCentralTarget = vCentral / ptsetActors->Num();
			pData->RefreshFollowState(bInit);

			if (bActorUpdated)
			{
				for (AActor* pActor : tsetTempActors)
				{
					USceneComponent* pComp = pActor->GetRootComponent();
					std::unordered_map<USceneComponent*, SLocationHelper>::iterator iter = m_mapLocationHelpers.find(pComp);
					if (iter != m_mapLocationHelpers.end())
					{
						m_mapLocationHelpers.erase(iter);
						pComp->TransformUpdated.RemoveAll(this);
					}
				}
			}
		}
	}
}

void UGroupCentralTargetMgr::OnActorTransformUpdated(USceneComponent* pUpdatedComponent,
	EUpdateTransformFlags eUpdateTransformFlags, ETeleportType eTeleport)
{
	std::unordered_map<USceneComponent*, SLocationHelper>::iterator iter = m_mapLocationHelpers.find(pUpdatedComponent);
	if (iter != m_mapLocationHelpers.end())
	{
		FGroupCentralData* pData = m_tmapCentralDatas.Find(iter->second.GroupName);
		if (!pData) return;

		FVector vLoc = pUpdatedComponent->GetComponentLocation();
		if (FVector::Distance(vLoc, iter->second.vLastLocation) >= pData->fRecenterPrecision)
			UpdateCentralTarget(iter->second.GroupName);
	}
}