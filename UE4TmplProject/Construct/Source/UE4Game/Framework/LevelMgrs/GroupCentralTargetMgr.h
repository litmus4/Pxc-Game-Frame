// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "VirtualGroupMgr.h"
#include <unordered_map>
#include "GroupCentralTargetMgr.generated.h"

class UVirtualGroupMgr;

USTRUCT()
struct FGrpCtrActorDirectInfo
{
	GENERATED_BODY()
public:
	float fMoveTime;

	UPROPERTY()
	UCurveFloat* pDynamicMover;
	float fDynamicMoveTime;
};

USTRUCT()
struct FGrpCtrActorViewInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AActor* pViewTarget;//为空时表示组中Actor本身就是ViewTarget

	float fBlendTime;
	EViewTargetBlendFunction eBlendFunc;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FGroupCentralDirectChangeDelegate, AActor*, pActor);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FGroupCentralViewChangeDelegate, AActor*, pActor, AActor*, pViewTarget);

USTRUCT()
struct FGroupCentralInfo
{
	GENERATED_BODY()
public:
	FGroupCentralInfo();

	void Init(const FName& xGroupName, float xFollowPrecision,
		float xFollowSpeed, float xFollowAccTime, float xFollowDecTime);
	void Init(FVirtualGroup* pGroup, float xFollowPrecision,
		float xFollowSpeed, float xFollowAccTime, float xFollowDecTime);

	void SetDirect(float fMoveTime, UCurveFloat* pDynamicMover);
	void SetView(float fBlendTime, EViewTargetBlendFunction eBlendFunc, AActor* pCentralVT);
	void AddActorDirectInfo(AActor* pActor, float fMoveTime, UCurveFloat* pDynamicMover);
	void AddActorViewInfo(AActor* pActor, AActor* pViewTarget, float fBlendTime, EViewTargetBlendFunction eBlendFunc);
	void ResetFloatings();
	//FLAGJK

	FName GroupName;
	float fFollowPrecision;
	float fFollowSpeed;
	float fFollowAccTime;
	float fFollowDecTime;

	float fDefaultMoveTime;
	UPROPERTY()
	UCurveFloat* pDefaultDynamicMover;
	UPROPERTY()
	TMap<AActor*, FGrpCtrActorDirectInfo> tmapActorDirectInfos;

	float fDefaultBlendTime;
	EViewTargetBlendFunction eDefaultBlendFunc;
	UPROPERTY()
	AActor* pCentralViewTarget;
	UPROPERTY()
	TMap<AActor*, FGrpCtrActorViewInfo> tmapActorViewInfos;

	FGroupCentralDirectChangeDelegate DeleDirectChanged;//参数为空表示指向了中心
	FGroupCentralViewChangeDelegate DeleViewChanged;//第一个参数为空表示看了中心

	UPROPERTY()
	TSet<AActor*> tsetBackActors;
	FVector vCentralTarget;

private:
	bool bFollowing;
	float fCurFollowSpeed;
	FVector vFollowTarget;
	float fAcceleration;
	float fDeceleration;

	bool bMoving;
	UPROPERTY()
	AActor* pCurDirect;//为空表示当前指向中心
	UPROPERTY()
	AActor* pLastDirect;
	FVector vDirectTarget;
	float fCurMoveTime;
	float fDynamicMoveMax;

	UPROPERTY()
	AActor* pCurView;//用原Actor，为空表示当前在看中心
};

/**
 * 
 */
UCLASS()
class UE4GAME_API UGroupCentralTargetMgr : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void SetCentralTarget(const FName& GroupName, float fFollowPrecision, float fFollowSpeed,
		float fFollowAccTime, float fFollowDecTime);

	//功能内部调用
	void UpdateCentralTarget(const FName& GroupName, UVirtualGroupMgr* pManager = nullptr, TSet<AActor*>* ptsetActors = nullptr);

private:
	struct SLocationHelper
	{
		FVector vLastLocation;
		FName GroupName;
	};

private:
	void OnActorTransformUpdated(USceneComponent* pUpdatedComponent,
		EUpdateTransformFlags eUpdateTransformFlags, ETeleportType eTeleport);

	UPROPERTY()
	TMap<FName, FGroupCentralInfo> m_tmapCentralInfos;

	std::unordered_map<USceneComponent*, SLocationHelper> m_mapLocationHelpers;
};
