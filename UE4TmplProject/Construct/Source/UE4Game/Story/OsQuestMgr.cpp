// Fill out your copyright notice in the Description page of Project Settings.


#include "Story/OsQuestMgr.h"

void UOsQuestMgr::InitDefault()
{
	//
}

void UOsQuestMgr::Init()
{
	//FLAGJK
}

void UOsQuestMgr::Release()
{
	m_mapQuestFsms.clear();
	m_tmapQuestStates.Empty();
}