// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "PrivateDefinitions/LevelManagersDef.h"
#include <string>
#include "RelativeTimeDilationMgr.generated.h"

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
};

/**
 * 
 */
UCLASS()
class UE4GAME_API URelativeTimeDilationMgr : public UObject
{
	GENERATED_BODY()
	
	/*FLAGJK
	 * TMap<int32, FTimeDilationData> m_tmapTimeDilations;
	 * std::map<int64, int32> m_mapHashExToUids; eLevel << 32 | uPartialHash, iUid
	 * std::unordered_map<ERTDilationLevel, int32> m_mapLevelNums;
	 */
};
