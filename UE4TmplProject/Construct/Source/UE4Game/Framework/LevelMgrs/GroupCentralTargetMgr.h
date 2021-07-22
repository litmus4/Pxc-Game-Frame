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
struct FGroupCentralData
{
	GENERATED_BODY()
public:
	enum EFloatingType
	{
		Direct = 0x1,
		View = 0x2
	};
#define AllCentralFloatings (EFloatingType::Direct | EFloatingType::View)

public:
	FGroupCentralData();

	void Init(const FName& xGroupName, float xRecenterPrecision, float xFollowPrecision,
		float xFollowSpeed, float xFollowAccTime, float xFollowDecTime);
	void Init(FVirtualGroup* pGroup, float xRecenterPrecision, float xFollowPrecision,
		float xFollowSpeed, float xFollowAccTime, float xFollowDecTime);

	void RefreshFollowState(bool bInit);
	void UpdateFollow(float fDeltaSeconds);
	FORCEINLINE FVector GetFollowTarget() { return vFollowTarget; }

	void SetDirect(float fMoveTime, UCurveFloat* pDynamicMover);
	void SetView(float fBlendTime, EViewTargetBlendFunction eBlendFunc, AActor* pCentralVT,
		APlayerController* xController, UGroupCentralTargetMgr* pMgr);
	void AddActorDirectInfo(AActor* pActor, float fMoveTime, UCurveFloat* pDynamicMover);
	void AddActorViewInfo(AActor* pActor, AActor* pViewTarget, float fBlendTime, EViewTargetBlendFunction eBlendFunc);
	void ResetFloatings();
	bool IsFloating(uint8 uFloatingFlags = AllCentralFloatings, bool bMovingOrBlending = true);
	bool IsActorFloating(AActor* pActor, uint8 uFloatingFlags = AllCentralFloatings);

	void MoveDirect(AActor* pActor);
	int32 UpdateDirect(float fDeltaSeconds);//-1:无 0:锁定 1:Moving 2:MoveEnd
	FORCEINLINE FVector GetDirectTarget() { return vDirectTarget; }
	void BlendView(AActor* pActor);
	void UpdateView(float fDeltaSeconds);
	void FlushEnd();

	FName GroupName;
	float fRecenterPrecision;
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
	APlayerController* pController;
	UPROPERTY()
	TMap<AActor*, FGrpCtrActorViewInfo> tmapActorViewInfos;

	FGroupCentralDirectChangeDelegate DeleDirectChanged;//参数为空表示指向了中心
	FGroupCentralViewChangeDelegate DeleViewChanged;//第一个参数为空表示看了中心

	UPROPERTY()
	TSet<AActor*> tsetBackActors;
	FVector vCentralTarget;
	bool bToBeResetted;

private:
	void DetermineDirectTarget(FVector* pvOut = nullptr);
	void OnViewChanged(UGroupCentralTargetMgr* pMgr = nullptr);

	bool bFollowing;
	FVector vFollowTarget;
	FVector vFollowVelocity;
	bool bFollowSpeed;
	bool bAccelerating;
	float fAcceleration;
	float fDeceleration;
	float fSOfAcc;
	float fSOfDec;
	float fAccTemp;
	float fDecTemp;

	bool bMoving;
	UPROPERTY()
	AActor* pCurDirect;//为空表示当前指向中心
	//UPROPERTY()
	//AActor* pLastDirect;
	//FVector vLastSnapShot;
	FVector vDirectTarget;
	float fCurMoveTime;
	float fDynamicMoveMax;

	UPROPERTY()
	AActor* pCurView;//用原Actor，为空表示当前在看中心
	FDelegateHandle DeleBlendHandle;
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
	void SetCentralTarget(const FName& GroupName, float fRecenterPrecision, float fFollowPrecision,
		float fFollowSpeed, float fFollowAccTime, float fFollowDecTime);

	//功能内部调用
	void UpdateCentralTarget(const FName& GroupName, UVirtualGroupMgr* pManager = nullptr, TSet<AActor*>* ptsetActors = nullptr);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ResetCentralTarget(const FName& GroupName);

	UFUNCTION(BlueprintPure, Category = Gameplay)
	bool GetCentralTarget(const FName& GroupName, FVector& vOut, bool bFollow = true);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void SetCentralDirect(const FName& GroupName, float fMoveTime, UCurveFloat* pDynamicMover,
		FGroupCentralDirectChangeDelegate DeleChanged);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void AddActorDirect(const FName& GroupName, AActor* pActor, float fMoveTime, UCurveFloat* pDynamicMover = nullptr);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void MoveCentralDirect(const FName& GroupName, AActor* pActor);

	UFUNCTION(BlueprintPure, Category = Gameplay)
	bool GetDirectTarget(const FName& GroupName, FVector& vOut);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void SetCentralView(const FName& GroupName, float fBlendTime, EViewTargetBlendFunction eBlendFunc,
		AActor* pCentralViewTarget, APlayerController* pController, FGroupCentralViewChangeDelegate DeleChanged);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void AddActorView(const FName& GroupName, AActor* pActor, AActor* pViewTarget,
		float fBlendTime, EViewTargetBlendFunction eBlendFunc);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void BlendCentralView(const FName& GroupName, AActor* pActor);

	UFUNCTION(BlueprintPure, Category = Gameplay)
	bool IsActorFloating(AActor* pActor);

	void Tick(float fDeltaSeconds);
	void Release();
	void OnCentralViewChanged(FGroupCentralData& Data);

private:
	struct SLocationHelper
	{
		FVector vLastLocation;
		TSet<FName> tsetGroupNames;
	};

private:
	void OnActorTransformUpdated(USceneComponent* pUpdatedComponent,
		EUpdateTransformFlags eUpdateTransformFlags, ETeleportType eTeleport);

	UPROPERTY()
	TMap<FName, FGroupCentralData> m_tmapCentralDatas;

	std::unordered_map<USceneComponent*, SLocationHelper> m_mapLocationHelpers;
};