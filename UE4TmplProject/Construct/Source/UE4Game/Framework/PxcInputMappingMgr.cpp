// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcInputMappingMgr.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/OtherTable/OtherTableCenter.h"
#include "Windows/PostWindowsApi.h"
#include "Windows/HideWindowsPlatformTypes.h"

void UPxcInputMappingMgr::LoadInputMappings()
{
	COtherTableCenter::GetInstance()->LoadInputActionNames();

	const std::map<int, CInputActionNameRow*>& mapInputActions = COtherTableCenter::GetInstance()->GetInputActionNameMap();
	std::map<int, CInputActionNameRow*>::const_iterator iter = mapInputActions.begin();
	for (; iter != mapInputActions.end(); iter++)
	{
		if (!iter->second) continue;

		FInputMappingQuad Quad;
		Quad.bAxis = iter->second->m_bAxis;
		FName CurActionName(ANSI_TO_TCHAR(iter->second->m_strActionName.c_str()));
		if (Quad.bAxis)
		{
			Quad.GamepadAxputMapping = FPxcAxputMapping(CurActionName, iter->second->m_bAxisPositive, false, 0);
			Quad.KeyboardAxputMapping = FPxcAxputMapping(CurActionName, iter->second->m_bAxisPositive, true, 0);
		}
		else
		{
			Quad.GamepadInputMapping = FPxcInputMapping(CurActionName, false, 0);
			Quad.KeyboardInputMapping = FPxcInputMapping(CurActionName, true, 0);
		}
		m_tarrInputMappings.Add(Quad);
	}
}

void UPxcInputMappingMgr::ReleaseInputMappings()
{
	m_tarrInputMappings.Empty();
}

TArray<FInputMappingQuad> UPxcInputMappingMgr::GetInputMappings()
{
	return m_tarrInputMappings;
}

FString UPxcInputMappingMgr::GetDisplayNameOfAction(const FName& ActionName, bool bAxisPositiveDir)
{
	std::string&& strActionName = TCHAR_TO_ANSI(*ActionName.ToString());
	CInputActionNameRow* pRow = COtherTableCenter::GetInstance()->GetInputActionNameRowByName(strActionName);
	if (!pRow)
	{
		pRow = COtherTableCenter::GetInstance()->GetInputActionNameRowByName(
			COtherTableCenter::LinkAxisName(strActionName, bAxisPositiveDir));
	}
	return (pRow ? ANSI_TO_TCHAR(pRow->m_strDisplayName.c_str()) : TEXT(""));
}