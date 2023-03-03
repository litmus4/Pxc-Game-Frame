// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/GroupCentralTargetMgr.h"
#include "../PxcGameMode.h"

FGroupCentralData::FGroupCentralData()
	: fRecenterPrecision(0.0f), fFollowPrecision(0.0f)
	, dFollowSpeed(0.0), fFollowAccTime(0.0f), fFollowDecTime(0.0f)
	, fDefaultMoveTime(-1.0f), pDefaultDynamicMover(nullptr)
	, fDefaultBlendTime(-1.0f), pCentralViewTarget(nullptr), pController(nullptr)
	, eDefaultBlendFunc(EViewTargetBlendFunction::VTBlend_Linear), bToBeResetted(false)
	, bFollowing(false), bFollowSpeed(false), bAccelerating(false)
	, dAcceleration(0.0), dDeceleration(0.0)
	, dSOfAcc(0.0), dSOfDec(0.0), dAccTemp(-1.0), dDecTemp(-1.0)
	, bMoving(false), pCurDirect(nullptr), fCurMoveTime(0.0f), fDynamicMoveMax(0.0f)
	, pCurView(nullptr)
{
	vCentralTarget = FVector::ZeroVector;
	vFollowTarget = FVector::ZeroVector;
	vFollowVelocity = FVector::ZeroVector;
	vDirectTarget = FVector::ZeroVector;
}

void FGroupCentralData::Init(const FName& xGroupName, float xRecenterPrecision, float xFollowPrecision,
	double xFollowSpeed, float xFollowAccTime, float xFollowDecTime)
{
	GroupName = xGroupName;
	fRecenterPrecision = FMath::Max(xRecenterPrecision, 0.0f);
	fFollowPrecision = FMath::Max(xFollowPrecision, 0.0f);
	dFollowSpeed = FMath::Max(xFollowSpeed, 0.0);
	fFollowAccTime = FMath::Max(xFollowAccTime, 0.0f);
	fFollowDecTime = FMath::Max(xFollowDecTime, 0.0f);

	//a=v/t
	dAcceleration = dFollowSpeed / fFollowAccTime;
	dDeceleration = dFollowSpeed / fFollowDecTime;
	//s=1/2*a*t^2
	dSOfAcc = dAcceleration * fFollowAccTime * fFollowAccTime * 0.5;
	dSOfDec = dDeceleration * fFollowDecTime * fFollowDecTime * 0.5;
}

void FGroupCentralData::Init(FVirtualGroup* pGroup, float xRecenterPrecision, float xFollowPrecision,
	double xFollowSpeed, float xFollowAccTime, float xFollowDecTime)
{
	check(pGroup);
	GroupName = pGroup->Name;
	fRecenterPrecision = FMath::Max(xRecenterPrecision, 0.0f);
	fFollowPrecision = FMath::Max(xFollowPrecision, 0.0f);
	dFollowSpeed = FMath::Max(xFollowSpeed, 0.0);
	fFollowAccTime = FMath::Max(xFollowAccTime, 0.0f);
	fFollowDecTime = FMath::Max(xFollowDecTime, 0.0f);

	tsetBackActors = pGroup->tsetActors;
	//a=v/t
	dAcceleration = dFollowSpeed / fFollowAccTime;
	dDeceleration = dFollowSpeed / fFollowDecTime;
	//s=1/2*a*t^2
	dSOfAcc = dAcceleration * fFollowAccTime * fFollowAccTime * 0.5;
	dSOfDec = dDeceleration * fFollowDecTime * fFollowDecTime * 0.5;
}

void FGroupCentralData::RefreshFollowState(bool bInit)
{
	if (bInit)
	{
		vFollowTarget = vCentralTarget;
		return;
	}

	double dDistance = FVector::Distance(vCentralTarget, vFollowTarget);
	if (!bFollowing)
	{
		if (dDistance >= fFollowPrecision)
		{
			bFollowing = true;
			bFollowSpeed = false;
			bAccelerating = true;
		}
		else
			return;
	}

	FVector vDir = (vCentralTarget - vFollowTarget).GetSafeNormal();
	if (!vDir.IsZero())
		vFollowVelocity = (bFollowSpeed ? vDir * vFollowVelocity.Size() : vDir);

	if (dDistance < fFollowPrecision)
	{
		bFollowing = false;
		vFollowTarget = vCentralTarget;
		vFollowVelocity = FVector::ZeroVector;
		bFollowSpeed = false;
		bAccelerating = false;
		dAccTemp = -1.0;
		dDecTemp = -1.0;
	}
	else if (dDistance <= dSOfDec)
	{
		bAccelerating = false;

		bool bResetVel = false;
		if (bFollowSpeed)
		{
			//a=v/t,s=1/2*a*t^2 => a=v/sqrt((s*2)/a) => a=v^2/(s*2)
			dDecTemp = vFollowVelocity.Size() * vFollowVelocity.Size() / (dDistance * 2.0);
			if (dDecTemp / dDeceleration < 0.667)
			{
				bResetVel = true;
				dDecTemp = -1.0;
			}
			else if (FMath::Abs(dDecTemp - dDeceleration) < 0.00001)
				dDecTemp = -1.0;
		}
		else
			bResetVel = true;

		if (bResetVel)
		{
			//v=at,s=1/2*a*t^2 => v=a*sqrt((s*2)/a)
			double dVelocity = FMath::Min(dDeceleration * FMath::Sqrt(dDistance * 2.0 / dDeceleration), dFollowSpeed);
			vFollowVelocity = vFollowVelocity.GetSafeNormal() * dVelocity;
			bFollowSpeed = true;
		}
	}
	else if (bAccelerating)
	{
		double dVelocity = (bFollowSpeed ? vFollowVelocity.Size() : 0.0);
		//t=(v-v0)/a,s=v0*t+1/2*a*t^2 当前速度加速到最大所需位移
		double dTime = (dFollowSpeed - dVelocity) / dAcceleration;
		double dCurS = dVelocity * dTime + dAcceleration * dTime * dTime * 0.5;
		if (dCurS > dDistance - dSOfDec)
		{
			dCurS = dDistance - dSOfDec;
			//a=(v-v0)/t,s=v0*t+1/2*a*t^2 => 解一元二次方程t=(-v0+sqrt(v0^2+a*s*2))/a =>
			//a=(v-v0)*a/(sqrt(v0^2+a*s*2)-v0) => v-v0=sqrt(v0^2+a*s*2)-v0 => a=(v^2-v0^2)/(s*2)
			dAccTemp = (dFollowSpeed * dFollowSpeed - dVelocity * dVelocity) / (dCurS * 2.0);
			if (FMath::Abs(dAccTemp - dAcceleration) < 0.00001)
				dAccTemp = -1.0;
		}
	}
}

void FGroupCentralData::UpdateFollow(float fDeltaSeconds)
{
	if (!bFollowing) return;

	double dDistance = FVector::Distance(vCentralTarget, vFollowTarget);
	if (dDistance < fFollowPrecision)
	{
		bFollowing = false;
		vFollowTarget = vCentralTarget;
		vFollowVelocity = FVector::ZeroVector;
		bFollowSpeed = false;
		bAccelerating = false;
		dAccTemp = -1.0;
		dDecTemp = -1.0;
	}
	else if (dDistance <= dSOfDec)
	{
		bAccelerating = false;

		double dDec = (dDecTemp >= -0.5 ? dDecTemp : dDeceleration);
		//s=v0*t-1/2*a*t^2
		double dDeltaS = vFollowVelocity.Size() * fDeltaSeconds - dDec * fDeltaSeconds * fDeltaSeconds * 0.5;
		dDeltaS = FMath::Clamp(dDeltaS, 0.0, dDistance);
		vFollowTarget += vFollowVelocity.GetSafeNormal() * dDeltaS;
		//v=v0-a*t
		double dVelocity = FMath::Max(vFollowVelocity.Size() - dDec * fDeltaSeconds, 0.0);
		if (dVelocity < 0.00001)
		{
			vFollowVelocity = vFollowVelocity.GetSafeNormal();
			bFollowSpeed = false;
			dDecTemp = -1.0;
		}
		else
			vFollowVelocity = vFollowVelocity.GetSafeNormal() * dVelocity;
	}
	else if (bAccelerating)
	{
		double dAcc = (dAccTemp >= -0.5 ? dAccTemp : dAcceleration);
		double dVelocity = (bFollowSpeed ? vFollowVelocity.Size() : 0.0);
		//t=(v-v0)/a,s=v0*t+1/2*a*t^2 当前速度加速到最大所需位移
		double dTime = (dFollowSpeed - dVelocity) / dAcc;
		double dCurS = dVelocity * dTime + dAcc * dTime * dTime * 0.5;
		//s=v0*t+1/2*a*t^2
		double dDeltaS = dVelocity * fDeltaSeconds + dAcc * fDeltaSeconds * fDeltaSeconds * 0.5;

		if (dDeltaS > dCurS)//超过了加速段
		{
			dTime = FMath::Max(fDeltaSeconds - dTime, 0.0);
			if (dDistance > dCurS + dSOfDec)//总距离包含了完整三段
			{
				dDeltaS = dDistance - dCurS - dSOfDec;//fDeltaS暂时设为匀速段全位移
				if (dTime > dDeltaS / dFollowSpeed)//超过了匀速段
				{
					dTime = FMath::Max(dTime - dDeltaS / dFollowSpeed, 0.0);
					if (dTime > 0.0)
					{
						//计算减速段 s=v0*t-1/2*a*t^2
						double dCurDecS = dFollowSpeed * dTime - dDeceleration * dTime * dTime * 0.5;
						dCurDecS = FMath::Clamp(dCurDecS, 0.0, dSOfDec);
						dDeltaS = dCurS + dDeltaS + dCurDecS;
						//v=v0-a*t
						dVelocity = FMath::Max(dFollowSpeed - dDeceleration * dTime, 0.0);
					}
					else
					{
						dDeltaS = dCurS + dDeltaS;
						dVelocity = dFollowSpeed;
					}
				}
				else//到达匀速段
				{
					dDeltaS = dCurS + dFollowSpeed * dTime;
					dVelocity = dFollowSpeed;
				}
			}
			else//总距离只包含加减速两段
			{
				if (dTime > 0.0)
				{
					//计算减速段 s=v0*t-1/2*a*t^2
					dDeltaS = dFollowSpeed * dTime - dDeceleration * dTime * dTime * 0.5;
					dDeltaS = FMath::Clamp(dDeltaS, 0.0, dSOfDec);
					dDeltaS = dCurS + dDeltaS;
					//v=v0-a*t
					dVelocity = FMath::Max(dFollowSpeed - dDeceleration * dTime, 0.0);
				}
				else
					dVelocity = dFollowSpeed;
			}
			bAccelerating = false;
			dAccTemp = -1.0;
		}

		vFollowTarget += vFollowVelocity.GetSafeNormal() * dDeltaS;
		if (bAccelerating)
		{
			//v=v0+a*t
			dVelocity += dAcc * fDeltaSeconds;
			if (dVelocity >= dFollowSpeed)
			{
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * dFollowSpeed;
				bAccelerating = false;
				dAccTemp = -1.0;
			}
			else
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * dVelocity;
		}
		else
		{
			if (dVelocity < 0.00001)
			{
				vFollowVelocity = vFollowVelocity.GetSafeNormal();
				bFollowSpeed = false;
			}
			else
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * dVelocity;
		}
	}
	else
	{
		bAccelerating = false;
		
		double dCurS = dDistance - dSOfDec;//匀速段位移
		double dChangedVel = -1.0;
		double dDeltaS = vFollowVelocity.Size() * fDeltaSeconds;

		if (dDeltaS > dCurS)//超过了匀速段
		{
			double dTime = FMath::Max(fDeltaSeconds - dCurS / vFollowVelocity.Size(), 0.0);
			if (dTime > 0.0)
			{
				//计算减速段 s=v0*t-1/2*a*t^2
				dDeltaS = vFollowVelocity.Size() * dTime - dDeceleration * dTime * dTime * 0.5;
				dDeltaS = FMath::Clamp(dDeltaS, 0.0, dSOfDec);
				dDeltaS = dCurS + dDeltaS;
				//v=v0-a*t
				dChangedVel = FMath::Max(vFollowVelocity.Size() - dDeceleration * dTime, 0.0);
			}
		}

		vFollowTarget += vFollowVelocity.GetSafeNormal() * dDeltaS;
		if (dChangedVel >= -0.5)
		{
			if (dChangedVel < 0.00001)
			{
				vFollowVelocity = vFollowVelocity.GetSafeNormal();
				bFollowSpeed = false;
			}
			else
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * dChangedVel;
		}
	}
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

	pCurDirect = nullptr;//默认指向中心
	vDirectTarget = vFollowTarget;
}

void FGroupCentralData::SetView(float fBlendTime, EViewTargetBlendFunction eBlendFunc, AActor* pCentralVT,
	APlayerController* xController, UGroupCentralTargetMgr* pMgr)
{
	verify(IsValid(pCentralVT));
	verify(IsValid(xController) && IsValid(xController->PlayerCameraManager));
	fDefaultBlendTime = FMath::Max(fBlendTime, 0.0f);
	eDefaultBlendFunc = eBlendFunc;
	pCentralViewTarget = pCentralVT;
	pController = xController;

	BlendView(nullptr, true);//默认看中心
	DeleBlendHandle = pController->PlayerCameraManager->OnBlendComplete().AddLambda([this, pMgr]() {
		OnViewChanged(pMgr);
	});
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
	verify(!pViewTarget || IsValid(pViewTarget));

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
	if (pController && pController->PlayerCameraManager)
		pController->PlayerCameraManager->OnBlendComplete().Remove(DeleBlendHandle);
	pController = nullptr;
	tmapActorViewInfos.Empty();
}

bool FGroupCentralData::IsFloating(uint8 uFloatingFlags, bool bMovingOrBlending)
{
	if (bMovingOrBlending)
	{
		if (uFloatingFlags & EFloatingType::Direct)
			if (bMoving) return true;
		if (uFloatingFlags & EFloatingType::View)
		{
			if (pController && pController->PlayerCameraManager &&
				pController->PlayerCameraManager->PendingViewTarget.Target)
				return true;
		}
	}
	else
	{
		if (uFloatingFlags & EFloatingType::Direct)
			if (fDefaultMoveTime >= -0.5f) return true;
		if (uFloatingFlags & EFloatingType::View)
			if (fDefaultBlendTime >= -0.5f) return true;
	}
	return false;
}

bool FGroupCentralData::IsActorFloating(AActor* pActor, uint8 uFloatingFlags)
{
	if (uFloatingFlags & EFloatingType::Direct)
		if (pActor == pCurDirect) return true;
	if (uFloatingFlags & EFloatingType::View)
		if (pActor == pCurView) return true;
	return false;
}

void FGroupCentralData::MoveDirect(AActor* pActor)
{
	if (pActor == pCurDirect)
		return;

	bMoving = true;
	pCurDirect = pActor;
	fCurMoveTime = 0.0f;
}

int32 FGroupCentralData::UpdateDirect(float fDeltaSeconds)
{
	if (fDefaultMoveTime < -0.5f) return -1;

	if (!bMoving)
	{
		DetermineDirectTarget();
		return 0;
	}

	float fMoveTime = fDefaultMoveTime;
	UCurveFloat* pDynamicMover = nullptr;
	bool bDefault = true;
	if (pCurDirect)
	{
		FGrpCtrActorDirectInfo* pInfo = tmapActorDirectInfos.Find(pCurDirect);
		if (pInfo)
		{
			fMoveTime = (IsValid(pInfo->pDynamicMover) ? pInfo->fDynamicMoveTime : pInfo->fMoveTime);
			pDynamicMover = (IsValid(pInfo->pDynamicMover) ? pInfo->pDynamicMover : nullptr);
			bDefault = false;
		}
	}
	if (bDefault && IsValid(pDefaultDynamicMover))
	{
		fMoveTime = fDynamicMoveMax;
		pDynamicMover = pDefaultDynamicMover;
	}

	float fLastMoveTime = fCurMoveTime;
	fCurMoveTime += fDeltaSeconds;
	if (fCurMoveTime < fMoveTime)
	{
		float fRatio = 0.0f;
		if (pDynamicMover)
		{
			float fCurValue = FMath::Clamp(pDynamicMover->GetFloatValue(FMath::Fmod(fCurMoveTime, fMoveTime)), 0.0f, 1.0f);
			float fLastValue = FMath::Clamp(pDynamicMover->GetFloatValue(FMath::Fmod(fLastMoveTime, fMoveTime)), 0.0f, 1.0f);
			fRatio = (fCurValue - fLastValue) / (1.0f - fLastValue);
		}
		else
			fRatio = fDeltaSeconds / (fMoveTime - fLastMoveTime);

		FVector vTarget;
		DetermineDirectTarget(&vTarget);
		vDirectTarget = FMath::Lerp(vDirectTarget, vTarget, fRatio);
	}
	else
	{
		DetermineDirectTarget();
		bMoving = false;
		DeleDirectChanged.ExecuteIfBound(pCurDirect);//TODOJK EventCenter
		return 2;
	}
	return 1;
}

void FGroupCentralData::BlendView(AActor* pActor, bool bForce)
{
	if ((pActor == pCurView && !bForce) || !pController)
		return;

	pCurView = pActor;

	AActor* pViewTarget = pCentralViewTarget;
	float fBlendTime = fDefaultBlendTime;
	EViewTargetBlendFunction eBlendFunc = eDefaultBlendFunc;
	if (pCurView)
	{
		pViewTarget = pCurView;
		FGrpCtrActorViewInfo* pInfo = tmapActorViewInfos.Find(pCurView);
		if (pInfo)
		{
			if (pInfo->pViewTarget) pViewTarget = pInfo->pViewTarget;
			fBlendTime = pInfo->fBlendTime;
			eBlendFunc = pInfo->eBlendFunc;
		}
	}
	pController->SetViewTargetWithBlend(pViewTarget, fBlendTime, eBlendFunc);
}

void FGroupCentralData::UpdateView(float fDeltaSeconds)
{
	if (fDefaultBlendTime < -0.5f) return;
	check(pCentralViewTarget);
	pCentralViewTarget->SetActorLocation(vFollowTarget);
}

void FGroupCentralData::FlushEnd()
{
	if (bMoving)
		DeleDirectChanged.ExecuteIfBound(pCurDirect);//TODOJK EventCenter

	if (IsValid(pController) && IsValid(pController->PlayerCameraManager) &&
		pController->PlayerCameraManager->PendingViewTarget.Target)
		OnViewChanged();
}

void FGroupCentralData::DetermineDirectTarget(FVector* pvOut)
{
	FVector& vTarget = (pvOut ? *pvOut : vDirectTarget);
	if (pCurDirect)
	{
		if (ensure(IsValid(pCurDirect)))
			vTarget = pCurDirect->GetActorLocation();
	}
	else
		vTarget = vFollowTarget;
}

void FGroupCentralData::OnViewChanged(UGroupCentralTargetMgr* pMgr)
{
	AActor* pViewTarget = pCentralViewTarget;
	if (pCurView)
	{
		FGrpCtrActorViewInfo* pInfo = tmapActorViewInfos.Find(pCurView);
		if (pInfo && pInfo->pViewTarget) pViewTarget = pInfo->pViewTarget;
	}
	DeleViewChanged.ExecuteIfBound(pCurView, pViewTarget);//TODOJK EventCenter
	
	if (pMgr)
		pMgr->OnCentralViewChanged(*this);
}

void UGroupCentralTargetMgr::SetCentralTarget(const FName& GroupName, float fRecenterPrecision, float fFollowPrecision,
	double dFollowSpeed, float fFollowAccTime, float fFollowDecTime)
{
	if (!m_pGM) m_pGM = CastChecked<APxcGameMode>(GetOuter());
	UVirtualGroupMgr* pManager = m_pGM->GetVirtualGroupMgr();
	check(pManager);
	FVirtualGroup* pGroup = pManager->GetGroup(GroupName);
	if (!pGroup || !pGroup->GetFeatureByUsage<FVirtGrpCentralFeature>(EVirtualGroupUsage::CentralTarget))
		return;

	if (m_tmapCentralDatas.Contains(GroupName))
		return;

	FGroupCentralData Data;
	Data.Init(pGroup, fRecenterPrecision, fFollowPrecision, dFollowSpeed, fFollowAccTime, fFollowDecTime);
	m_tmapCentralDatas.Add(GroupName, Data);

	for (AActor* pActor : pGroup->tsetActors)
	{
		USceneComponent* pComp = pActor->GetRootComponent();
		std::unordered_map<USceneComponent*, SLocationHelper>::iterator iter = m_mapLocationHelpers.find(pComp);
		if (iter == m_mapLocationHelpers.end())
		{
			SLocationHelper Helper;
			Helper.vLastLocation = pActor->GetActorLocation();
			Helper.tsetGroupNames.Add(GroupName);
			m_mapLocationHelpers.insert(std::make_pair(pComp, Helper));
			pComp->TransformUpdated.AddUObject(this, &UGroupCentralTargetMgr::OnActorTransformUpdated);
		}
		else
			iter->second.tsetGroupNames.Add(GroupName);
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
			if (!m_pGM) m_pGM = CastChecked<APxcGameMode>(GetOuter());
			pManager = m_pGM->GetVirtualGroupMgr();
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

			FVector vCentral(0.0, 0.0, 0.0);
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
						Helper.tsetGroupNames.Add(GroupName);
						m_mapLocationHelpers.insert(std::make_pair(pComp, Helper));
						pComp->TransformUpdated.AddUObject(this, &UGroupCentralTargetMgr::OnActorTransformUpdated);
					}
					else
					{
						iter->second.tsetGroupNames.Add(GroupName);
						tsetTempActors.Remove(pActor);
					}
				}
			}
			pData->vCentralTarget = vCentral / ptsetActors->Num();
			pData->RefreshFollowState(bInit);

			if (bActorUpdated)
			{
				for (AActor* pActor : tsetTempActors)
				{
					//正在Direct、Moving或View的Actor不能删除，请在外部做判断
					verify(!pData->IsActorFloating(pActor));

					USceneComponent* pComp = pActor->GetRootComponent();
					std::unordered_map<USceneComponent*, SLocationHelper>::iterator iter = m_mapLocationHelpers.find(pComp);
					if (iter == m_mapLocationHelpers.end())
						continue;
					
					iter->second.tsetGroupNames.Remove(GroupName);
					if (iter->second.tsetGroupNames.Num() == 0)
					{
						m_mapLocationHelpers.erase(iter);
						pComp->TransformUpdated.RemoveAll(this);
					}
				}
			}
		}
	}
}

void UGroupCentralTargetMgr::ResetCentralTarget(const FName& GroupName)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (!pData || pData->bToBeResetted)
		return;

	if (pData->IsFloating())
	{
		pData->bToBeResetted = true;
		return;
	}

	pData->ResetFloatings();
	for (AActor* pActor : pData->tsetBackActors)
	{
		USceneComponent* pComp = pActor->GetRootComponent();
		std::unordered_map<USceneComponent*, SLocationHelper>::iterator iter = m_mapLocationHelpers.find(pComp);
		if (iter == m_mapLocationHelpers.end())
			continue;

		iter->second.tsetGroupNames.Remove(GroupName);
		if (iter->second.tsetGroupNames.Num() == 0)
		{
			m_mapLocationHelpers.erase(iter);
			pComp->TransformUpdated.RemoveAll(this);
		}
	}
	m_tmapCentralDatas.Remove(GroupName);
}

bool UGroupCentralTargetMgr::GetCentralTarget(const FName& GroupName, FVector& vOut, bool bFollow)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (pData)
	{
		vOut = (bFollow ? pData->GetFollowTarget() : pData->vCentralTarget);
		return true;
	}
	return false;
}

void UGroupCentralTargetMgr::SetCentralDirect(const FName& GroupName, float fMoveTime, UCurveFloat* pDynamicMover,
	FGroupCentralDirectChangeDelegate DeleChanged)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (!pData || pData->IsFloating(FGroupCentralData::Direct) || pData->bToBeResetted)
		return;
	pData->SetDirect(fMoveTime, pDynamicMover);
	pData->DeleDirectChanged = DeleChanged;
}

void UGroupCentralTargetMgr::AddActorDirect(const FName& GroupName, AActor* pActor, float fMoveTime, UCurveFloat* pDynamicMover)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (!pData || pData->IsFloating(FGroupCentralData::Direct) || pData->bToBeResetted)
		return;
	if (pData->tsetBackActors.Contains(pActor))
		pData->AddActorDirectInfo(pActor, fMoveTime, pDynamicMover);
}

void UGroupCentralTargetMgr::MoveCentralDirect(const FName& GroupName, AActor* pActor)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (!pData || !pData->IsFloating(FGroupCentralData::Direct, false) || pData->bToBeResetted)
		return;

	if (pData->tsetBackActors.Contains(pActor))
		pData->MoveDirect(pActor);
	else
		pData->MoveDirect(nullptr);
}

bool UGroupCentralTargetMgr::GetDirectTarget(const FName& GroupName, FVector& vOut)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (pData && pData->IsFloating(FGroupCentralData::Direct, false))
	{
		vOut = pData->GetDirectTarget();
		return true;
	}
	return false;
}

void UGroupCentralTargetMgr::SetCentralView(const FName& GroupName, float fBlendTime, EViewTargetBlendFunction eBlendFunc,
	AActor* pCentralViewTarget, APlayerController* pController, FGroupCentralViewChangeDelegate DeleChanged)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (!pData || pData->IsFloating(FGroupCentralData::View) || pData->bToBeResetted)
		return;
	pData->SetView(fBlendTime, eBlendFunc, pCentralViewTarget, pController, this);
	pData->DeleViewChanged = DeleChanged;
}

void UGroupCentralTargetMgr::AddActorView(const FName& GroupName, AActor* pActor, AActor* pViewTarget,
	float fBlendTime, EViewTargetBlendFunction eBlendFunc)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (!pData || pData->IsFloating(FGroupCentralData::View) || pData->bToBeResetted)
		return;
	if (pData->tsetBackActors.Contains(pActor))
		pData->AddActorViewInfo(pActor, pViewTarget, fBlendTime, eBlendFunc);
}

void UGroupCentralTargetMgr::BlendCentralView(const FName& GroupName, AActor* pActor)
{
	FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
	if (!pData || !pData->IsFloating(FGroupCentralData::View, false) || pData->bToBeResetted)
		return;

	if (pData->tsetBackActors.Contains(pActor))
		pData->BlendView(pActor);
	else
		pData->BlendView(nullptr);
}

bool UGroupCentralTargetMgr::IsActorFloating(AActor* pActor)
{
	verify(IsValid(pActor));

	for (auto& Pair : m_tmapCentralDatas)
	{
		if (Pair.Value.IsActorFloating(pActor))
			return true;
	}
	return false;
}

void UGroupCentralTargetMgr::Tick(float fDeltaSeconds)
{
	for (auto& Pair : m_tmapCentralDatas)
	{
		FGroupCentralData& Data = Pair.Value;
		Data.UpdateFollow(fDeltaSeconds);

		if (Data.UpdateDirect(fDeltaSeconds) == 2)
		{
			if (Data.bToBeResetted && !Data.IsFloating())
			{
				Data.bToBeResetted = false;
				ResetCentralTarget(Pair.Key);
				continue;
			}
		}

		Data.UpdateView(fDeltaSeconds);
	}
}

void UGroupCentralTargetMgr::Release()
{
	for (auto& Pair : m_tmapCentralDatas)
		Pair.Value.FlushEnd();
}

void UGroupCentralTargetMgr::OnCentralViewChanged(FGroupCentralData& Data)
{
	if (Data.bToBeResetted && !Data.IsFloating())
	{
		Data.bToBeResetted = false;
		ResetCentralTarget(Data.GroupName);
	}
}

void UGroupCentralTargetMgr::OnActorTransformUpdated(USceneComponent* pUpdatedComponent,
	EUpdateTransformFlags eUpdateTransformFlags, ETeleportType eTeleport)
{
	std::unordered_map<USceneComponent*, SLocationHelper>::iterator iter = m_mapLocationHelpers.find(pUpdatedComponent);
	if (iter != m_mapLocationHelpers.end())
	{
		FVector vLoc = pUpdatedComponent->GetComponentLocation();
		double dDis = FVector::Distance(vLoc, iter->second.vLastLocation);
		for (FName& GroupName : iter->second.tsetGroupNames)
		{
			FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
			if (!pData) continue;
			if (dDis >= pData->fRecenterPrecision)
				UpdateCentralTarget(GroupName);
		}
	}
}