// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "QuestState.generated.h"

class CQuestRow;

/**
 * 
 */
UCLASS()
class UEGAME_API UQuestState : public UObject
{
	GENERATED_BODY()
	
public:
	UQuestState();

	void Init(int32 iQuestID);
	void Init(CQuestRow* pQuestRow);

	FORCEINLINE int32 GetQuestID() { return m_iQuestID; }
	FORCEINLINE CQuestRow* GetQuestRow() { return m_pQuestRow; }

	UPROPERTY()
	int32 m_iRuntimeHeadLevel = -1;

protected:
	UPROPERTY(BlueprintReadOnly)
	int32 m_iQuestID;

	CQuestRow* m_pQuestRow;
};