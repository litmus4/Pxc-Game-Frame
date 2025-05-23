// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "PxcUtil/StateMachine.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

#include "QuestState.generated.h"

class CQuestRow;

/**
 * 
 */
UCLASS()
class UEGAME_API UQuestState : public UObject, public PxcUtil::IState
{
	GENERATED_BODY()
	
public:
	UQuestState();

	void Init(int32 iQuestID);
	void Init(CQuestRow* pQuestRow);

	virtual void OnEnter() override;
	virtual void OnExit() override;

	FORCEINLINE int32 GetQuestID() { return m_iQuestID; }
	FORCEINLINE CQuestRow* GetQuestRow() { return m_pQuestRow; }

	UPROPERTY()
	int32 m_iRuntimeHeadLevel = -1;

protected:
	UPROPERTY(BlueprintReadOnly)
	int32 m_iQuestID;

	CQuestRow* m_pQuestRow;
};