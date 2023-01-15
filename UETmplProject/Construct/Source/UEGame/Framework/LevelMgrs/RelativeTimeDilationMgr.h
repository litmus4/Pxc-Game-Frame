// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrivateDefinitions/LevelManagersDef.h"
#include "SpecialLevelMgrBase.h"
#include <map>
#include <unordered_map>
#include "RelativeTimeDilationMgr.generated.h"

class APxcGameMode;

USTRUCT()
struct FTimeDilationInfo
{
	GENERATED_BODY()
public:
	FTimeDilationInfo();

	void InitGlobal(float xDuration, float xBlendInTime, float xBlendOutTime,
		float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority);
	void InitGroup(const FName& GroupName, float xDuration, float xBlendInTime, float xBlendOutTime,
		float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority, bool xIgnoreParent);
	void InitActor(AActor* pActor, float xDuration, float xBlendInTime, float xBlendOutTime,
		float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority, bool xIgnoreParent);

	void UpdateDilation(float fDeltaSeconds, float& fOutDilation);
	bool IsFinished() const;
	bool IsLooping() const;

	friend uint32 GetTypeHash(const FTimeDilationInfo& Info);
	uint32 GetPartialHash(bool bNoPriority = false) const;
	int64 GetHashEx(bool bNoPriority = false);
	static int64 MakeHashEx(ERTDilationLevel eLevel, const FName& GroupName, AActor* pActor, int32 iPriority = 0);

	float fDuration;//<-0.5 >0.0
	float fBlendInTime;
	float fBlendOutTime;

	float fStaticDilation;
	UPROPERTY()
	UCurveFloat* pDynamicDilation;

	ERTDilationLevel eLevel;
	FName AffectGroupName;
	UPROPERTY()
	AActor* pAffectActor;
	bool bIgnoreParent;

	int32 iPriority;

private:
	void InitUniversal(float xDuration, float xBlendInTime, float xBlendOutTime,
		float xStaticDilation, UCurveFloat* xDynamicDilation, int32 xPriority);

	float fCurBlendInTime;//<-0.5 >0.0
	float fCurBlendOutTime;//<-0.5 >0.0
	float fRemainingTime;//<-0.5 >0.0
	float fElapsedTime;
	float fDynamicMax;
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FTimeDilationEndDelegate, bool, bCanceled);

USTRUCT()
struct FTimeDilationData
{
	GENERATED_BODY()
public:
	int32 iUid;

	UPROPERTY()
	FTimeDilationInfo Info;

	FTimeDilationEndDelegate DeleEnded;
};

/**
 * 
 */
UCLASS()
class UEGAME_API URelativeTimeDilationMgr : public USpecialLevelMgrBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = Gameplay, meta = (fDuration = "1.0", fStaticDilation = "1.0", iPriority = "1"))
	int32 SetGlobalDilation(float fDuration, float fBlendInTime, float fBlendOutTime,
		float fStaticDilation, UCurveFloat* pDynamicDilation, int32 iPriority, FTimeDilationEndDelegate DeleEnded);

	UFUNCTION(BlueprintCallable, Category = Gameplay, meta = (fDuration = "1.0", fStaticDilation = "1.0", iPriority = "1"))
	int32 SetGroupDilation(const FName& GroupName, float fDuration, float fBlendInTime, float fBlendOutTime,
		float fStaticDilation, UCurveFloat* pDynamicDilation, int32 iPriority, bool bIgnoreParent, FTimeDilationEndDelegate DeleEnded);

	UFUNCTION(BlueprintCallable, Category = Gameplay, meta = (fDuration = "1.0", fStaticDilation = "1.0", iPriority = "1"))
	int32 SetActorDilation(AActor* pActor, float fDuration, float fBlendInTime, float fBlendOutTime,
		float fStaticDilation, UCurveFloat* pDynamicDilation, int32 iPriority, bool bIgnoreParent, FTimeDilationEndDelegate DeleEnded);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ResetDilationByUid(int32 iUid, bool bCanceled = true, bool bDisableCallback = false);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ResetDilationByGroupName(const FName& GroupName);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void ResetDilationByActor(AActor* pActor);

	virtual void Tick(float fDeltaSeconds) override;
	virtual void Release() override;

private:
	struct SGroupPreferred
	{
		int32 iPriority;
		TSet<AActor*>* ptsetActors;
	};

	struct SActorPreferred
	{
		int32 iPriority;
		float fDilation;
		float fDilationInGroup;
	};

private:
	UPROPERTY()
	TMap<int32, FTimeDilationData> m_tmapTimeDilations;

	std::map<int64, int32> m_mapHashExToUids;
	std::unordered_map<int64, int32> m_mapHashExnpNums;

	APxcGameMode* m_pGM = nullptr;
};