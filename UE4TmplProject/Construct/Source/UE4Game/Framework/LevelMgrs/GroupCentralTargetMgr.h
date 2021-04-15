// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GroupCentralTargetMgr.generated.h"

USTRUCT()
struct FGrpCtrActorDirectInfo
{
	GENERATED_BODY()
public:
	float fMoveTime;

	UPROPERTY()
	UCurveFloat* pDynamicMover;
};

USTRUCT()
struct FGrpCtrActorViewInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AActor* pViewTarget;//Ϊ��ʱ��ʾ����Actor�������ViewTarget

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

	void Init(const FName& xGroupName);
	//FLAGJK

	FName GroupName;

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

	FGroupCentralDirectChangeDelegate DeleDirectChanged;//����Ϊ�ձ�ʾָ��������
	FGroupCentralViewChangeDelegate DeleViewChanged;//��һ������Ϊ�ձ�ʾ��������

private:
	FVector vCentralTarget;

	bool bDirecting;
	UPROPERTY()
	AActor* pCurDirect;//Ϊ�ձ�ʾ��ǰָ������
	UPROPERTY()
	AActor* pLastDirect;
	FVector vDirectTarget;
	float fCurMoveTime;

	bool bViewing;
	UPROPERTY()
	AActor* pCurView;//��ԭActor��Ϊ�ձ�ʾ��ǰ�ڿ�����
};

/**
 * 
 */
UCLASS()
class UE4GAME_API UGroupCentralTargetMgr : public UObject
{
	GENERATED_BODY()
	
};
