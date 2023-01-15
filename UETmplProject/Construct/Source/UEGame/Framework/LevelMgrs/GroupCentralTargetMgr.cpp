// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/LevelMgrs/GroupCentralTargetMgr.h"
#include "../PxcGameMode.h"

FGroupCentralData::FGroupCentralData()
	: fRecenterPrecision(0.0f), fFollowPrecision(0.0f)
	, fFollowSpeed(0.0f), fFollowAccTime(0.0f), fFollowDecTime(0.0f)
	, fDefaultMoveTime(-1.0f), pDefaultDynamicMover(nullptr)
	, fDefaultBlendTime(-1.0f), pCentralViewTarget(nullptr), pController(nullptr)
	, eDefaultBlendFunc(EViewTargetBlendFunction::VTBlend_Linear), bToBeResetted(false)
	, bFollowing(false), bFollowSpeed(false), bAccelerating(false)
	, fAcceleration(0.0f), fDeceleration(0.0f)
	, fSOfAcc(0.0f), fSOfDec(0.0f), fAccTemp(-1.0f), fDecTemp(-1.0f)
	, bMoving(false), pCurDirect(nullptr), fCurMoveTime(0.0f), fDynamicMoveMax(0.0f)
	, pCurView(nullptr)
{
	vCentralTarget = FVector::ZeroVector;
	vFollowTarget = FVector::ZeroVector;
	vFollowVelocity = FVector::ZeroVector;
	vDirectTarget = FVector::ZeroVector;
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
	//s=1/2*a*t^2
	fSOfAcc = fAcceleration * fFollowAccTime * fFollowAccTime * 0.5f;
	fSOfDec = fDeceleration * fFollowDecTime * fFollowDecTime * 0.5f;
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
	//s=1/2*a*t^2
	fSOfAcc = fAcceleration * fFollowAccTime * fFollowAccTime * 0.5f;
	fSOfDec = fDeceleration * fFollowDecTime * fFollowDecTime * 0.5f;
}

void FGroupCentralData::RefreshFollowState(bool bInit)
{
	if (bInit)
	{
		vFollowTarget = vCentralTarget;
		return;
	}

	float fDistance = FVector::Distance(vCentralTarget, vFollowTarget);
	if (!bFollowing)
	{
		if (fDistance >= fFollowPrecision)
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

	if (fDistance < fFollowPrecision)
	{
		bFollowing = false;
		vFollowTarget = vCentralTarget;
		vFollowVelocity = FVector::ZeroVector;
		bFollowSpeed = false;
		bAccelerating = false;
		fAccTemp = -1.0f;
		fDecTemp = -1.0f;
	}
	else if (fDistance <= fSOfDec)
	{
		bAccelerating = false;

		bool bResetVel = false;
		if (bFollowSpeed)
		{
			//a=v/t,s=1/2*a*t^2 => a=v/sqrt((s*2)/a) => a=v^2/(s*2)
			fDecTemp = vFollowVelocity.Size() * vFollowVelocity.Size() / (fDistance * 2.0f);
			if (fDecTemp / fDeceleration < 0.667f)
			{
				bResetVel = true;
				fDecTemp = -1.0f;
			}
			else if (FMath::Abs(fDecTemp - fDeceleration) < 0.00001f)
				fDecTemp = -1.0f;
		}
		else
			bResetVel = true;

		if (bResetVel)
		{
			//v=at,s=1/2*a*t^2 => v=a*sqrt((s*2)/a)
			float fVelocity = FMath::Min(fDeceleration * FMath::Sqrt(fDistance * 2.0f / fDeceleration), fFollowSpeed);
			vFollowVelocity = vFollowVelocity.GetSafeNormal() * fVelocity;
			bFollowSpeed = true;
		}
	}
	else if (bAccelerating)
	{
		float fVelocity = (bFollowSpeed ? vFollowVelocity.Size() : 0.0f);
		//t=(v-v0)/a,s=v0*t+1/2*a*t^2 当前速度加速到最大所需位移
		float fTime = (fFollowSpeed - fVelocity) / fAcceleration;
		float fCurS = fVelocity * fTime + fAcceleration * fTime * fTime * 0.5f;
		if (fCurS > fDistance - fSOfDec)
		{
			fCurS = fDistance - fSOfDec;
			//a=(v-v0)/t,s=v0*t+1/2*a*t^2 => 解一元二次方程t=(-v0+sqrt(v0^2+a*s*2))/a =>
			//a=(v-v0)*a/(sqrt(v0^2+a*s*2)-v0) => v-v0=sqrt(v0^2+a*s*2)-v0 => a=(v^2-v0^2)/(s*2)
			fAccTemp = (fFollowSpeed * fFollowSpeed - fVelocity * fVelocity) / (fCurS * 2.0f);
			if (FMath::Abs(fAccTemp - fAcceleration) < 0.00001f)
				fAccTemp = -1.0f;
		}
	}
}

void FGroupCentralData::UpdateFollow(float fDeltaSeconds)
{
	if (!bFollowing) return;

	float fDistance = FVector::Distance(vCentralTarget, vFollowTarget);
	if (fDistance < fFollowPrecision)
	{
		bFollowing = false;
		vFollowTarget = vCentralTarget;
		vFollowVelocity = FVector::ZeroVector;
		bFollowSpeed = false;
		bAccelerating = false;
		fAccTemp = -1.0f;
		fDecTemp = -1.0f;
	}
	else if (fDistance <= fSOfDec)
	{
		bAccelerating = false;

		float fDec = (fDecTemp >= -0.5f ? fDecTemp : fDeceleration);
		//s=v0*t-1/2*a*t^2
		float fDeltaS = vFollowVelocity.Size() * fDeltaSeconds - fDec * fDeltaSeconds * fDeltaSeconds * 0.5f;
		fDeltaS = FMath::Clamp(fDeltaS, 0.0f, fDistance);
		vFollowTarget += vFollowVelocity.GetSafeNormal() * fDeltaS;
		//v=v0-a*t
		float fVelocity = FMath::Max(vFollowVelocity.Size() - fDec * fDeltaSeconds, 0.0f);
		if (fVelocity < 0.00001f)
		{
			vFollowVelocity = vFollowVelocity.GetSafeNormal();
			bFollowSpeed = false;
			fDecTemp = -1.0f;
		}
		else
			vFollowVelocity = vFollowVelocity.GetSafeNormal() * fVelocity;
	}
	else if (bAccelerating)
	{
		float fAcc = (fAccTemp >= -0.5f ? fAccTemp : fAcceleration);
		float fVelocity = (bFollowSpeed ? vFollowVelocity.Size() : 0.0f);
		//t=(v-v0)/a,s=v0*t+1/2*a*t^2 当前速度加速到最大所需位移
		float fTime = (fFollowSpeed - fVelocity) / fAcc;
		float fCurS = fVelocity * fTime + fAcc * fTime * fTime * 0.5f;
		//s=v0*t+1/2*a*t^2
		float fDeltaS = fVelocity * fDeltaSeconds + fAcc * fDeltaSeconds * fDeltaSeconds * 0.5f;

		if (fDeltaS > fCurS)//超过了加速段
		{
			fTime = FMath::Max(fDeltaSeconds - fTime, 0.0f);
			if (fDistance > fCurS + fSOfDec)//总距离包含了完整三段
			{
				fDeltaS = fDistance - fCurS - fSOfDec;//fDeltaS暂时设为匀速段全位移
				if (fTime > fDeltaS / fFollowSpeed)//超过了匀速段
				{
					fTime = FMath::Max(fTime - fDeltaS / fFollowSpeed, 0.0f);
					if (fTime > 0.0f)
					{
						//计算减速段 s=v0*t-1/2*a*t^2
						float fCurDecS = fFollowSpeed * fTime - fDeceleration * fTime * fTime * 0.5f;
						fCurDecS = FMath::Clamp(fCurDecS, 0.0f, fSOfDec);
						fDeltaS = fCurS + fDeltaS + fCurDecS;
						//v=v0-a*t
						fVelocity = FMath::Max(fFollowSpeed - fDeceleration * fTime, 0.0f);
					}
					else
					{
						fDeltaS = fCurS + fDeltaS;
						fVelocity = fFollowSpeed;
					}
				}
				else//到达匀速段
				{
					fDeltaS = fCurS + fFollowSpeed * fTime;
					fVelocity = fFollowSpeed;
				}
			}
			else//总距离只包含加减速两段
			{
				if (fTime > 0.0f)
				{
					//计算减速段 s=v0*t-1/2*a*t^2
					fDeltaS = fFollowSpeed * fTime - fDeceleration * fTime * fTime * 0.5f;
					fDeltaS = FMath::Clamp(fDeltaS, 0.0f, fSOfDec);
					fDeltaS = fCurS + fDeltaS;
					//v=v0-a*t
					fVelocity = FMath::Max(fFollowSpeed - fDeceleration * fTime, 0.0f);
				}
				else
					fVelocity = fFollowSpeed;
			}
			bAccelerating = false;
			fAccTemp = -1.0f;
		}

		vFollowTarget += vFollowVelocity.GetSafeNormal() * fDeltaS;
		if (bAccelerating)
		{
			//v=v0+a*t
			fVelocity += fAcc * fDeltaSeconds;
			if (fVelocity >= fFollowSpeed)
			{
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * fFollowSpeed;
				bAccelerating = false;
				fAccTemp = -1.0f;
			}
			else
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * fVelocity;
		}
		else
		{
			if (fVelocity < 0.00001f)
			{
				vFollowVelocity = vFollowVelocity.GetSafeNormal();
				bFollowSpeed = false;
			}
			else
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * fVelocity;
		}
	}
	else
	{
		bAccelerating = false;
		
		float fCurS = fDistance - fSOfDec;//匀速段位移
		float fChangedVel = -1.0f;
		float fDeltaS = vFollowVelocity.Size() * fDeltaSeconds;

		if (fDeltaS > fCurS)//超过了匀速段
		{
			float fTime = FMath::Max(fDeltaSeconds - fCurS / vFollowVelocity.Size(), 0.0f);
			if (fTime > 0.0f)
			{
				//计算减速段 s=v0*t-1/2*a*t^2
				fDeltaS = vFollowVelocity.Size() * fTime - fDeceleration * fTime * fTime * 0.5f;
				fDeltaS = FMath::Clamp(fDeltaS, 0.0f, fSOfDec);
				fDeltaS = fCurS + fDeltaS;
				//v=v0-a*t
				fChangedVel = FMath::Max(vFollowVelocity.Size() - fDeceleration * fTime, 0.0f);
			}
		}

		vFollowTarget += vFollowVelocity.GetSafeNormal() * fDeltaS;
		if (fChangedVel >= -0.5f)
		{
			if (fChangedVel < 0.00001f)
			{
				vFollowVelocity = vFollowVelocity.GetSafeNormal();
				bFollowSpeed = false;
			}
			else
				vFollowVelocity = vFollowVelocity.GetSafeNormal() * fChangedVel;
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
	float fFollowSpeed, float fFollowAccTime, float fFollowDecTime)
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
	Data.Init(pGroup, fRecenterPrecision, fFollowPrecision, fFollowSpeed, fFollowAccTime, fFollowDecTime);
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
		float fDis = FVector::Distance(vLoc, iter->second.vLastLocation);
		for (FName& GroupName : iter->second.tsetGroupNames)
		{
			FGroupCentralData* pData = m_tmapCentralDatas.Find(GroupName);
			if (!pData) continue;
			if (fDis >= pData->fRecenterPrecision)
				UpdateCentralTarget(GroupName);
		}
	}
}