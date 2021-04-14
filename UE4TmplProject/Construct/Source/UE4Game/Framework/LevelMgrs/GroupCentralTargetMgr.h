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
	AActor* pViewTarget;//Ϊ��ʱ��ʾ����Actor�������ViewTarget

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

	FGroupCentralViewChangeDelegate DeleViewChanged;//��һ������Ϊ�ձ�ʾ�ƶ���������

private:
	FVector vCentralTarget;

	bool bViewing;
	UPROPERTY()
	AActor* pCurView;//Ϊ�ձ�ʾ��ǰ�ڿ�����

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
