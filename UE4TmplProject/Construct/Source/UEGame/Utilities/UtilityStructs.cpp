// Fill out your copyright notice in the Description page of Project Settings.


#include "Utilities/UtilityStructs.h"
#include "Framework/PxcAssistantSubsystem.h"
#include "PxcUtil/IDPool.h"

int32 FIdentifiedLessStack::PushValue(float fValue)
{
	auto RetPair = set.insert(fValue);
	while (!RetPair.second)
		RetPair = set.insert(fValue += fDiffPrecision);

	std::string&& strKey = TCHAR_TO_ANSI(*sTypeKey);
	PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetIdentifiedStackUidPool(strKey);
	int32 iUid = pUidPool->Generate();
	tmapUidToIt.Add(iUid, RetPair.first);
	return iUid;
}

void FIdentifiedLessStack::PopValue(int32 iUid)
{
	std::set<float>::iterator* pIter = tmapUidToIt.Find(iUid);
	if (pIter)
	{
		set.erase(*pIter);
		tmapUidToIt.Remove(iUid);
		std::string&& strKey = TCHAR_TO_ANSI(*sTypeKey);
		PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetIdentifiedStackUidPool(strKey);
		pUidPool->Free(iUid);
	}
}

int32 FIdentifiedGreaterStack::PushValue(float fValue)
{
	auto RetPair = set.insert(fValue);
	while (!RetPair.second)
		RetPair = set.insert(fValue -= fDiffPrecision);

	std::string&& strKey = TCHAR_TO_ANSI(*sTypeKey);
	PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetIdentifiedStackUidPool(strKey);
	int32 iUid = pUidPool->Generate();
	tmapUidToIt.Add(iUid, RetPair.first);
	return iUid;
}

void FIdentifiedGreaterStack::PopValue(int32 iUid)
{
	std::set<float>::iterator* pIter = tmapUidToIt.Find(iUid);
	if (pIter)
	{
		set.erase(*pIter);
		tmapUidToIt.Remove(iUid);
		std::string&& strKey = TCHAR_TO_ANSI(*sTypeKey);
		PxcUtil::CIDPool* pUidPool = UPxcAssistantSubsystem::GetInstance()->GetIdentifiedStackUidPool(strKey);
		pUidPool->Free(iUid);
	}
}