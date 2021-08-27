// Fill out your copyright notice in the Description page of Project Settings.


#include "Story/QuestState.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/StoryTable/StoryTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

UQuestState::UQuestState()
	: m_iQuestID(-1)
	, m_pQuestRow(nullptr)
{
	//
}

void UQuestState::Init(int32 iQuestID)
{
	//
}

void UQuestState::Init(CQuestRow* pQuestRow)
{
	check(pQuestRow);
	m_iQuestID = pQuestRow->m_iID;
	m_pQuestRow = pQuestRow;
}