// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GroupCentralTargetMgr.generated.h"

USTRUCT()
struct FGrpCtrActorViewInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AActor* pViewTarget;//为空时表示组中Actor本身就是ViewTarget

	float fDefaultBlendTime;
	EViewTargetBlendFunction eDefaultBlendFunc;
};

DECLARE_DYNAMIC_DELEGATE_TwoParams(FGroupCentralViewChangeDelegate, AActor*, pActor, AActor*, pViewTarget);

USTRUCT()
struct FGroupCentralInfo
{
	GENERATED_BODY()
public:
	FGroupCentralInfo();

	FName GroupName;

	UPROPERTY()
	AActor* pCentralViewTarget;

	UPROPERTY()
	TMap<AActor*, FGrpCtrActorViewInfo> tmapActorViewInfos;

	FGroupCentralViewChangeDelegate DeleViewChanged;//第一个参数为空表示移动到了中心

private:
	FVector vCentralTarget;

	bool bViewing;
	UPROPERTY()
	AActor* pCurView;//为空表示当前在看中心

	//FLAGJK
};

/**
 * 
 */
UCLASS()
class UE4GAME_API UGroupCentralTargetMgr : public UObject
{
	GENERATED_BODY()
	
};
