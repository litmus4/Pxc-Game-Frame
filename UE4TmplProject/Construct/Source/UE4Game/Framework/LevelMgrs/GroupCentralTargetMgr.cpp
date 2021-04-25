// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/GroupCentralTargetMgr.h"
#include "../PxcGameMode.h"

FGroupCentralInfo::FGroupCentralInfo()
	: bFollowing(false), fFollowPrecision(0.0f), fFollowSpeed(0.0f)
	, fFollowAccTime(0.0f), fFollowDecTime(0.0f)
	, fDefaultMoveTime(-1.0f), pDefaultDynamicMover(nullptr)
	, fDefaultBlendTime(-1.0f), pCentralViewTarget(nullptr)
	, eDefaultBlendFunc(EViewTargetBlendFunction::VTBlend_Linear)
	, fCurFollowSpeed(0.0f), fAcceleration(0.0f), fDeceleration(0.0f)
	, bMoving(false), pCurDirect(nullptr), pLastDirect(nullptr)
	, fCurMoveTime(0.0f), fDynamicMoveMax(0.0f), pCurView(nullptr)
{
	vCentralTarget = FVector(0.0f, 0.0f, 0.0f);
	vFollowTarget = FVector(0.0f, 0.0f, 0.0f);
	vDirectTarget = FVector(0.0f, 0.0f, 0.0f);
}

void FGroupCentralInfo::Init(const FName& xGroupName, float xFollowPrecision,
	float xFollowSpeed, float xFollowAccTime, float xFollowDecTime)
{
	GroupName = xGroupName;
	fFollowPrecision = FMath::Max(xFollowPrecision, 0.0f);
	fFollowSpeed = FMath::Max(xFollowSpeed, 0.0f);
	fFollowAccTime = FMath::Max(xFollowAccTime, 0.0f);
	fFollowDecTime = FMath::Max(xFollowDecTime, 0.0f);

	//a=v/t
	fAcceleration = fFollowSpeed / fFollowAccTime;
	fDeceleration = fFollowSpeed / fFollowDecTime;
}

void FGroupCentralInfo::Init(FVirtualGroup* pGroup, float xFollowPrecision,
	float xFollowSpeed, float xFollowAccTime, float xFollowDecTime)
{
	check(pGroup);
	GroupName = pGroup->Name;
	fFollowPrecision = FMath::Max(xFollowPrecision, 0.0f);
	fFollowSpeed = FMath::Max(xFollowSpeed, 0.0f);
	fFollowAccTime = FMath::Max(xFollowAccTime, 0.0f);
	fFollowDecTime = FMath::Max(xFollowDecTime, 0.0f);

	tsetBackActors = pGroup->tsetActors;
	//a=v/t
	fAcceleration = fFollowSpeed / fFollowAccTime;
	fDeceleration = fFollowSpeed / fFollowDecTime;
}

void FGroupCentralInfo::SetDirect(float fMoveTime, UCurveFloat* pDynamicMover)
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

void FGroupCentralInfo::SetView(float fBlendTime, EViewTargetBlendFunction eBlendFunc, AActor* pCentralVT)
{
	verify(IsValid(pCentralVT));
	fDefaultBlendTime = FMath::Max(fBlendTime, 0.0f);
	eDefaultBlendFunc = eBlendFunc;
	pCentralViewTarget = pCentralVT;

	pCurView = nullptr;//默认看中心
}

void FGroupCentralInfo::AddActorDirectInfo(AActor* pActor, float fMoveTime, UCurveFloat* pDynamicMover)
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

void FGroupCentralInfo::AddActorViewInfo(AActor* pActor, AActor* pViewTarget, float fBlendTime, EViewTargetBlendFunction eBlendFunc)
{
	check(IsValid(pActor));
	verify(IsValid(pViewTarget));

	FGrpCtrActorViewInfo Info;
	Info.pViewTarget = pViewTarget;
	Info.fBlendTime = FMath::Max(fBlendTime, 0.0f);
	Info.eBlendFunc = eBlendFunc;
	tmapActorViewInfos.Add(pActor, Info);
}

void FGroupCentralInfo::ResetFloatings()
{
	fDefaultMoveTime = -1.0f;
	pDefaultDynamicMover = nullptr;
	tmapActorDirectInfos.Empty();

	fDefaultBlendTime = -1.0f;
	eDefaultBlendFunc = EViewTargetBlendFunction::VTBlend_Linear;
	pCentralViewTarget = nullptr;
	tmapActorViewInfos.Empty();
}

void UGroupCentralTargetMgr::SetCentralTarget(const FName& GroupName, float fFollowPrecision, float fFollowSpeed,
	float fFollowAccTime, float fFollowDecTime)
{
	UVirtualGroupMgr* pManager = CastChecked<APxcGameMode>(GetOuter())->GetVirtualGroupMgr();
	check(pManager);
	FVirtualGroup* pGroup = pManager->GetGroup(GroupName);
	if (!pGroup || !pGroup->GetFeatureByUsage<FVirtGrpCentralFeature>(EVirtualGroupUsage::CentralTarget))
		return;

	if (m_tmapCentralInfos.Contains(GroupName))
		return;

	FGroupCentralInfo Info;
	Info.Init(pGroup, fFollowPrecision, fFollowSpeed, fFollowAccTime, fFollowDecTime);
	m_tmapCentralInfos.Add(GroupName, Info);

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
	if (!bActorUpdated)
	{
		if (!pManager)
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
		FGroupCentralInfo* pInfo = m_tmapCentralInfos.Find(GroupName);
		if (pInfo)
		{
			TSet<AActor*> tsetTempActors;
			if (bActorUpdated)
			{
				tsetTempActors = pInfo->tsetBackActors;
				pInfo->tsetBackActors.Empty();
			}

			FVector vCentral(0.0f, 0.0f, 0.0f);
			for (AActor* pActor : *ptsetActors)
			{
				vCentral += pActor->GetActorLocation();

				if (bActorUpdated)
				{
					pInfo->tsetBackActors.Add(pActor);
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
			pInfo->vCentralTarget = vCentral / ptsetActors->Num();

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
	//FLAGJK
}