// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcInputMappingMgr.h"
#include "Kismet/KismetInputLibrary.h"
#include "Framework/PxcGameConfig.h"
#include "Framework/PxcBlueprintLibrary.h"

#include "Windows/AllowWindowsPlatformTypes.h"
#include "Windows/PreWindowsApi.h"
#include "DataTables/TextTable/TextTableCenter.h"
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

	if (pRow)
	{
		std::string&& strName = CTextTableCenter::GetInstance()->GetText(pRow->m_iDisplayTextID);
		return UTF8_TO_TCHAR(strName.c_str());
	}
	return TEXT("");
}

FString UPxcInputMappingMgr::GetDisplayNameOfQuad(const FInputMappingQuad& Quad)
{
	if (Quad.bAxis)
		return GetDisplayNameOfAction(Quad.GamepadAxputMapping.AxisName, Quad.GamepadAxputMapping.bPositiveDir);
	return GetDisplayNameOfAction(Quad.GamepadInputMapping.ActionName, false);
}

bool UPxcInputMappingMgr::IsActionModifyForbidden(const FName& ActionName, bool bAxisPositiveDir, bool bKeyboard)
{
	std::string&& strActionName = TCHAR_TO_ANSI(*ActionName.ToString());
	CInputActionNameRow* pRow = COtherTableCenter::GetInstance()->GetInputActionNameRowByName(strActionName);
	if (!pRow)
	{
		pRow = COtherTableCenter::GetInstance()->GetInputActionNameRowByName(
			COtherTableCenter::LinkAxisName(strActionName, bAxisPositiveDir));
	}
	if (pRow)
		return (bKeyboard ? pRow->m_bKeyboardModifyForbidden : pRow->m_bGamepadModifyForbidden);
	return true;
}

bool UPxcInputMappingMgr::IsQuadModifyForbidden(const FInputMappingQuad& Quad, bool bKeyboard)
{
	if (Quad.bAxis)
	{
		const FPxcAxputMapping* pAxputMapping = (bKeyboard ? &Quad.KeyboardAxputMapping : &Quad.GamepadAxputMapping);
		return IsActionModifyForbidden(pAxputMapping->AxisName, pAxputMapping->bPositiveDir, bKeyboard);
	}
	const FPxcInputMapping* pInputMapping = (bKeyboard ? &Quad.KeyboardInputMapping : &Quad.GamepadInputMapping);
	return IsActionModifyForbidden(pInputMapping->ActionName, false, bKeyboard);
}

bool UPxcInputMappingMgr::IsQuadHasKey(const FInputMappingQuad& Quad, bool bKeyboard)
{
	if (Quad.bAxis)
	{
		const FPxcAxputMapping* pAxputMapping = (bKeyboard ? &Quad.KeyboardAxputMapping : &Quad.GamepadAxputMapping);
		return !pAxputMapping->KeyName.IsNone();
	}
	const FPxcInputMapping* pInputMapping = (bKeyboard ? &Quad.KeyboardInputMapping : &Quad.GamepadInputMapping);
	return !pInputMapping->KeyName.IsNone();
}

bool UPxcInputMappingMgr::ModifyGamepadInputMapping(const FInputActionKeyMapping& KeyMapping, bool bCombine, EGamepadModifierKey eModifierKey,
	FName& ConflictUniActionName)
{
	check(!KeyMapping.ActionName.IsNone() && KeyMapping.Key.IsValid() && KeyMapping.Key.IsGamepadKey());
	check(!KeyMapping.bShift && !KeyMapping.bCtrl && !KeyMapping.bAlt && !KeyMapping.bCmd);
	if (KeyMapping.Key.IsAxis1D())
		return false;

	FPxcInputMapping NewInputMapping;
	NewInputMapping.ActionName = KeyMapping.ActionName;
	NewInputMapping.KeyName = KeyMapping.Key.GetFName();
	if (bCombine)
		NewInputMapping.uModifierCode = 1 << ((int32)eModifierKey + 4);

	FInputMappingQuad* pModifiedQuad = nullptr;
	FInputMappingQuad* pConflictQuad = nullptr;
	for (FInputMappingQuad& Quad : m_tarrInputMappings)
	{
		if (!Quad.bAxis && Quad.GamepadInputMapping.ActionName == KeyMapping.ActionName)
			pModifiedQuad = &Quad;
		if (Quad.bAxis ? Quad.GamepadAxputMapping.IsSame(NewInputMapping) : Quad.GamepadInputMapping.IsSame(NewInputMapping))
			pConflictQuad = &Quad;
	}
	if (!pModifiedQuad || pModifiedQuad == pConflictQuad/*同一个操作按下了原来的按键*/)
		return false;
	pModifiedQuad->GamepadInputMapping = NewInputMapping;

	UInputSettings* pSetting = UInputSettings::GetInputSettings();
	UPxcGameConfig* pConfig = GetMutableDefault<UPxcGameConfig>();

	FInputActionKeyMapping FirstKeyMapping;
	FGamepadCombineMapping FirstCombineMapping;
	int32 iResult = FindFirstDevisedAcMapping(KeyMapping.ActionName, false, true, FirstKeyMapping, FirstCombineMapping);
	if (iResult)
	{
		pSetting->RemoveActionMapping(FirstKeyMapping, false);
		if (iResult < 0)
			pConfig->RemoveGamepadCombineMapping(FirstCombineMapping);
	}

	pSetting->AddActionMapping(KeyMapping, !pConflictQuad);
	if (bCombine)
	{
		FGamepadCombineMapping NewCombineMapping;
		NewCombineMapping.ActionName = KeyMapping.ActionName;
		NewCombineMapping.MainKey = KeyMapping.Key;
		NewCombineMapping.eModifierKey = eModifierKey;
		pConfig->AddGamepadCombineMapping(NewCombineMapping);
	}

	FName DeleConflictUniName;
	if (pConflictQuad)
	{
		if (pConflictQuad->bAxis)
		{
			FInputAxisKeyMapping FirAxKeyMapping;
			if (FindFirstDevisedAxMapping(pConflictQuad->GamepadAxputMapping.AxisName, pConflictQuad->GamepadAxputMapping.bPositiveDir,
				false, FirAxKeyMapping))
				pSetting->RemoveAxisMapping(FirAxKeyMapping);

			pConflictQuad->GamepadAxputMapping.KeyName = NAME_None;
			pConflictQuad->GamepadAxputMapping.iFloatPositive = 0;
			std::string&& strUniActionName = COtherTableCenter::LinkAxisName(TCHAR_TO_ANSI(*pConflictQuad->GamepadAxputMapping.AxisName.ToString()),
				pConflictQuad->GamepadAxputMapping.bPositiveDir);
			ConflictUniActionName = DeleConflictUniName = ANSI_TO_TCHAR(strUniActionName.c_str());
		}
		else
		{
			iResult = FindFirstDevisedAcMapping(pConflictQuad->GamepadInputMapping.ActionName, false, true, FirstKeyMapping, FirstCombineMapping);
			if (iResult)
			{
				pSetting->RemoveActionMapping(FirstKeyMapping);
				if (iResult < 0)
					pConfig->RemoveGamepadCombineMapping(FirstCombineMapping);
			}

			pConflictQuad->GamepadInputMapping.KeyName = NAME_None;
			pConflictQuad->GamepadInputMapping.uModifierCode = 0;
			ConflictUniActionName = DeleConflictUniName = pConflictQuad->GamepadInputMapping.ActionName;
		}
	}

	pSetting->SaveKeyMappings();
	pConfig->SaveConfig();

	DeleInputMappingModified.Broadcast(*pModifiedQuad, false, DeleConflictUniName);
	//TODOJK EventCenter
	return true;
}

bool UPxcInputMappingMgr::ModifyKeyboardInputMapping(const FInputActionKeyMapping& KeyMapping, FName& ConflictUniActionName)
{
	check(!KeyMapping.ActionName.IsNone() && KeyMapping.Key.IsValid() && UKismetInputLibrary::Key_IsKeyboardKey(KeyMapping.Key));

	FPxcInputMapping NewInputMapping;
	NewInputMapping.ActionName = KeyMapping.ActionName;
	NewInputMapping.KeyName = KeyMapping.Key.GetFName();
	if (KeyMapping.bShift) NewInputMapping.uModifierCode |= 1;
	if (KeyMapping.bCtrl) NewInputMapping.uModifierCode |= 1 << 1;
	if (KeyMapping.bAlt) NewInputMapping.uModifierCode |= 1 << 2;
	if (KeyMapping.bCmd) NewInputMapping.uModifierCode |= 1 << 3;

	FInputMappingQuad* pModifiedQuad = nullptr;
	FInputMappingQuad* pConflictQuad = nullptr;
	for (FInputMappingQuad& Quad : m_tarrInputMappings)
	{
		if (!Quad.bAxis && Quad.KeyboardInputMapping.ActionName == KeyMapping.ActionName)
			pModifiedQuad = &Quad;
		if (Quad.bAxis ? Quad.KeyboardAxputMapping.IsSame(NewInputMapping) : Quad.KeyboardInputMapping.IsSame(NewInputMapping))
			pConflictQuad = &Quad;
	}
	if (!pModifiedQuad || pModifiedQuad == pConflictQuad/*同一个操作按下了原来的按键*/)
		return false;
	pModifiedQuad->KeyboardInputMapping = NewInputMapping;

	UInputSettings* pSetting = UInputSettings::GetInputSettings();

	FInputActionKeyMapping FirstKeyMapping;
	FGamepadCombineMapping FirstCombineMapping;
	int32 iResult = FindFirstDevisedAcMapping(KeyMapping.ActionName, true, false, FirstKeyMapping, FirstCombineMapping);
	if (iResult)
		pSetting->RemoveActionMapping(FirstKeyMapping, false);
	pSetting->AddActionMapping(KeyMapping, !pConflictQuad);

	FName DeleConflictUniName;
	if (pConflictQuad)
	{
		if (pConflictQuad->bAxis)
		{
			FInputAxisKeyMapping FirAxKeyMapping;
			if (FindFirstDevisedAxMapping(pConflictQuad->KeyboardAxputMapping.AxisName, pConflictQuad->KeyboardAxputMapping.bPositiveDir,
				true, FirAxKeyMapping))
				pSetting->RemoveAxisMapping(FirAxKeyMapping);

			pConflictQuad->KeyboardAxputMapping.KeyName = NAME_None;
			pConflictQuad->KeyboardAxputMapping.iFloatPositive = 0;
			std::string&& strUniActionName = COtherTableCenter::LinkAxisName(TCHAR_TO_ANSI(*pConflictQuad->KeyboardAxputMapping.AxisName.ToString()),
				pConflictQuad->KeyboardAxputMapping.bPositiveDir);
			ConflictUniActionName = DeleConflictUniName = ANSI_TO_TCHAR(strUniActionName.c_str());
		}
		else
		{
			iResult = FindFirstDevisedAcMapping(pConflictQuad->KeyboardInputMapping.ActionName, true, false, FirstKeyMapping, FirstCombineMapping);
			if (iResult)
				pSetting->RemoveActionMapping(FirstKeyMapping);

			pConflictQuad->KeyboardInputMapping.KeyName = NAME_None;
			pConflictQuad->KeyboardInputMapping.uModifierCode = 0;
			ConflictUniActionName = DeleConflictUniName = pConflictQuad->KeyboardInputMapping.ActionName;
		}
	}

	pSetting->SaveKeyMappings();
	DeleInputMappingModified.Broadcast(*pModifiedQuad, true, DeleConflictUniName);
	//TODOJK EventCenter
	return true;
}

enum EGpAxputOppositeType
{
	NormalToFloat,
	FloatToNormal,
	FloatToFloat,
	FloatSwap
};

bool UPxcInputMappingMgr::ModifyGamepadAxputMapping(const FInputAxisKeyMapping& KeyMapping, bool bFloatPositive,
	FName& OppositeUniActionName, TArray<FName>& tarrConflictUniActionNames)
{
	check(!KeyMapping.AxisName.IsNone() && KeyMapping.Key.IsValid() && KeyMapping.Key.IsGamepadKey());

	FPxcAxputMapping NewAxputMapping;
	NewAxputMapping.AxisName = KeyMapping.AxisName;
	NewAxputMapping.bPositiveDir = (KeyMapping.Scale >= 0);
	NewAxputMapping.KeyName = KeyMapping.Key.GetFName();
	NewAxputMapping.iFloatPositive = (KeyMapping.Key.IsAxis1D() ? (bFloatPositive ? 1 : -1) : 0);

	FInputMappingQuad* pModifiedQuad = nullptr;
	FInputMappingQuad* pOppositeQuad = nullptr;
	EGpAxputOppositeType eOppositeType = EGpAxputOppositeType::FloatSwap;
	TArray<FInputMappingQuad*> tarrConflictQuads;
	for (FInputMappingQuad& Quad : m_tarrInputMappings)
	{
		if (Quad.bAxis && Quad.GamepadAxputMapping.AxisName == KeyMapping.AxisName)
		{
			if (Quad.GamepadAxputMapping.bPositiveDir == NewAxputMapping.bPositiveDir)
				pModifiedQuad = &Quad;
			else
			{
				if (Quad.GamepadAxputMapping.KeyName == NewAxputMapping.KeyName)//普通按键冲突或swap
				{
					if (Quad.GamepadAxputMapping.iFloatPositive != 0)//swap
						pOppositeQuad = &Quad;
				}
				else if (NewAxputMapping.iFloatPositive != 0)//普通或float改为float
				{
					pOppositeQuad = &Quad;
					if (Quad.GamepadAxputMapping.iFloatPositive != 0)//float改为float
						eOppositeType = EGpAxputOppositeType::FloatToFloat;
					else
						eOppositeType = EGpAxputOppositeType::NormalToFloat;
				}
				else if (Quad.GamepadAxputMapping.iFloatPositive != 0)//float改为普通
				{
					pOppositeQuad = &Quad;
					eOppositeType = EGpAxputOppositeType::FloatToNormal;
				}
			}
		}
		if (Quad.bAxis ? Quad.GamepadAxputMapping.KeyName == NewAxputMapping.KeyName : Quad.GamepadInputMapping.IsSame(NewAxputMapping))
			tarrConflictQuads.Add(&Quad);
	}

	if (!pModifiedQuad) return false;
	int32 i = 0;
	while (i < tarrConflictQuads.Num())
	{
		FInputMappingQuad* pCurConfQuad = tarrConflictQuads[i];
		if (pCurConfQuad == pModifiedQuad)
		{
			if (pCurConfQuad->GamepadAxputMapping.iFloatPositive == NewAxputMapping.iFloatPositive)//同一个操作按下了原来的按键
				return false;
			else
				tarrConflictQuads.RemoveAt(i);
		}
		else if (pCurConfQuad == pOppositeQuad)
			tarrConflictQuads.RemoveAt(i);
		else i++;
	}

	FName DeleOppositeUniName;
	pModifiedQuad->GamepadAxputMapping = NewAxputMapping;
	if (pOppositeQuad)
	{
		switch (eOppositeType)
		{
		case EGpAxputOppositeType::NormalToFloat:
		case EGpAxputOppositeType::FloatToFloat:
			pOppositeQuad->GamepadAxputMapping.KeyName = NewAxputMapping.KeyName;
			pOppositeQuad->GamepadAxputMapping.iFloatPositive = (bFloatPositive ? -1 : 1);
			break;
		case EGpAxputOppositeType::FloatToNormal:
			pOppositeQuad->GamepadAxputMapping.KeyName = NAME_None;
			pOppositeQuad->GamepadAxputMapping.iFloatPositive = 0;
			break;
		case EGpAxputOppositeType::FloatSwap:
			pOppositeQuad->GamepadAxputMapping.iFloatPositive = (bFloatPositive ? -1 : 1);
		}
		std::string&& strUniActionName = COtherTableCenter::LinkAxisName(TCHAR_TO_ANSI(*pOppositeQuad->GamepadAxputMapping.AxisName.ToString()),
			pOppositeQuad->GamepadAxputMapping.bPositiveDir);
		OppositeUniActionName = DeleOppositeUniName = ANSI_TO_TCHAR(strUniActionName.c_str());
	}

	UInputSettings* pSetting = UInputSettings::GetInputSettings();
	//UPxcGameConfig* pConfig = GetMutableDefault<UPxcGameConfig>();

	FInputAxisKeyMapping FirstKeyMapping;
	if (FindFirstDevisedAxMapping(KeyMapping.AxisName, NewAxputMapping.bPositiveDir, false, FirstKeyMapping))
		pSetting->RemoveAxisMapping(FirstKeyMapping, false);
	if (pOppositeQuad && eOppositeType == EGpAxputOppositeType::NormalToFloat)
	{
		if (FindFirstDevisedAxMapping(pOppositeQuad->GamepadAxputMapping.AxisName, pOppositeQuad->GamepadAxputMapping.bPositiveDir,
			false, FirstKeyMapping))
			pSetting->RemoveAxisMapping(FirstKeyMapping, false);
	}
	FInputAxisKeyMapping KeyMappingToSet = KeyMapping;
	KeyMappingToSet.Scale = (NewAxputMapping.bPositiveDir == bFloatPositive ? 1.0f : -1.0f);
	pSetting->AddAxisMapping(KeyMappingToSet, tarrConflictQuads.Num() == 0);

	TArray<FName> tarrDeleConflictUniNames;
	for (FInputMappingQuad* pCurConfQuad : tarrConflictQuads)
	{
		if (pCurConfQuad->bAxis)
		{
			if (FindFirstDevisedAxMapping(pCurConfQuad->GamepadAxputMapping.AxisName, pCurConfQuad->GamepadAxputMapping.bPositiveDir,
				false, FirstKeyMapping))
				pSetting->RemoveAxisMapping(FirstKeyMapping);

			pCurConfQuad->GamepadAxputMapping.KeyName = NAME_None;
			pCurConfQuad->GamepadAxputMapping.iFloatPositive = 0;
			FName&& UniActionName = ANSI_TO_TCHAR(COtherTableCenter::LinkAxisName(
				TCHAR_TO_ANSI(*pCurConfQuad->GamepadAxputMapping.AxisName.ToString()), pCurConfQuad->GamepadAxputMapping.bPositiveDir).c_str());
			tarrDeleConflictUniNames.Add(UniActionName);
			tarrConflictUniActionNames.Add(UniActionName);
		}
		else
		{
			FInputActionKeyMapping FirAcKeyMapping;
			FGamepadCombineMapping FirAcCombineMapping;
			int32 iResult = FindFirstDevisedAcMapping(pCurConfQuad->GamepadInputMapping.ActionName, false, true, FirAcKeyMapping, FirAcCombineMapping);
			if (iResult)
			{
				pSetting->RemoveActionMapping(FirAcKeyMapping);
				//if (iResult < 0)//理论上方向按键是统一单键的，手柄上不会与组合键冲突
				//	pConfig->RemoveGamepadCombineMapping(FirAcCombineMapping);
			}

			pCurConfQuad->GamepadInputMapping.KeyName = NAME_None;
			pCurConfQuad->GamepadInputMapping.uModifierCode = 0;
			tarrDeleConflictUniNames.Add(pCurConfQuad->GamepadInputMapping.ActionName);
			tarrConflictUniActionNames.Add(pCurConfQuad->GamepadInputMapping.ActionName);
		}
	}

	pSetting->SaveKeyMappings();
	//pConfig->SaveConfig();

	DeleGamepadAxputMappingModified.Broadcast(*pModifiedQuad, false, DeleOppositeUniName, tarrDeleConflictUniNames);
	//TODOJK EventCenter
	return true;
}

bool UPxcInputMappingMgr::ModifyKeyboardAxputMapping(const FInputAxisKeyMapping& KeyMapping, FName& ConflictUniActionName)
{
	check(!KeyMapping.AxisName.IsNone() && KeyMapping.Key.IsValid() && UKismetInputLibrary::Key_IsKeyboardKey(KeyMapping.Key));

	FPxcAxputMapping NewAxputMapping;
	NewAxputMapping.AxisName = KeyMapping.AxisName;
	NewAxputMapping.bPositiveDir = (KeyMapping.Scale >= 0);
	NewAxputMapping.KeyName = KeyMapping.Key.GetFName();
	NewAxputMapping.iFloatPositive = 0;

	FInputMappingQuad* pModifiedQuad = nullptr;
	FInputMappingQuad* pConflictQuad = nullptr;
	for (FInputMappingQuad& Quad : m_tarrInputMappings)
	{
		if (Quad.bAxis && Quad.KeyboardAxputMapping.AxisName == KeyMapping.AxisName &&
			Quad.KeyboardAxputMapping.bPositiveDir == NewAxputMapping.bPositiveDir)
			pModifiedQuad = &Quad;
		if (Quad.bAxis ? Quad.KeyboardAxputMapping.KeyName == NewAxputMapping.KeyName : Quad.KeyboardInputMapping.IsSame(NewAxputMapping))
			pConflictQuad = &Quad;
	}
	if (!pModifiedQuad || pModifiedQuad == pConflictQuad/*同一个操作按下了原来的按键*/)
		return false;
	pModifiedQuad->KeyboardAxputMapping = NewAxputMapping;

	UInputSettings* pSetting = UInputSettings::GetInputSettings();

	FInputAxisKeyMapping FirstKeyMapping;
	if (FindFirstDevisedAxMapping(KeyMapping.AxisName, NewAxputMapping.bPositiveDir, true, FirstKeyMapping))
		pSetting->RemoveAxisMapping(FirstKeyMapping, false);
	pSetting->AddAxisMapping(KeyMapping, !pConflictQuad);

	FName DeleConflictUniName;
	if (pConflictQuad)
	{
		if (pConflictQuad->bAxis)
		{
			if (FindFirstDevisedAxMapping(pConflictQuad->KeyboardAxputMapping.AxisName, pConflictQuad->KeyboardAxputMapping.bPositiveDir,
				true, FirstKeyMapping))
				pSetting->RemoveAxisMapping(FirstKeyMapping);

			pConflictQuad->KeyboardAxputMapping.KeyName = NAME_None;
			pConflictQuad->KeyboardAxputMapping.iFloatPositive = 0;
			std::string&& strUniActionName = COtherTableCenter::LinkAxisName(TCHAR_TO_ANSI(*pConflictQuad->KeyboardAxputMapping.AxisName.ToString()),
				pConflictQuad->KeyboardAxputMapping.bPositiveDir);
			ConflictUniActionName = DeleConflictUniName = ANSI_TO_TCHAR(strUniActionName.c_str());
		}
		else
		{
			FInputActionKeyMapping FirAcKeyMapping;
			FGamepadCombineMapping FirAcCombineMapping;
			int32 iResult = FindFirstDevisedAcMapping(pConflictQuad->KeyboardInputMapping.ActionName, true, false, FirAcKeyMapping, FirAcCombineMapping);
			if (iResult)
				pSetting->RemoveActionMapping(FirAcKeyMapping);

			pConflictQuad->KeyboardInputMapping.KeyName = NAME_None;
			pConflictQuad->KeyboardInputMapping.uModifierCode = 0;
			ConflictUniActionName = DeleConflictUniName = pConflictQuad->KeyboardInputMapping.ActionName;
		}
	}

	pSetting->SaveKeyMappings();
	DeleInputMappingModified.Broadcast(*pModifiedQuad, true, DeleConflictUniName);
	//TODOJK EventCenter
	return true;
}

int32 UPxcInputMappingMgr::FindFirstDevisedAcMapping(const FName& ActionName, bool bDevisedKeyboard, bool bGamepadCombBoth,
	FInputActionKeyMapping& OutMapping, FGamepadCombineMapping& OutGamepadCombMapping)
{
	TArray<FInputActionKeyMapping> tarrMappings;
	TArray<FGamepadCombineMapping> tarrGamepadCombMappings;
	UPxcBlueprintLibrary::Key_GetAllActionMappingByName(ActionName, tarrMappings, tarrGamepadCombMappings);
	if (tarrMappings.Num() > 0 || tarrGamepadCombMappings.Num() > 0)
	{
		return UPxcBlueprintLibrary::Key_GetIndexedDevisedAcMapping(tarrMappings, tarrGamepadCombMappings,
			bDevisedKeyboard, 0, bGamepadCombBoth, OutMapping, OutGamepadCombMapping);
	}
	return 0;
}

bool UPxcInputMappingMgr::FindFirstDevisedAxMapping(const FName& AxisName, bool bPositiveDir, bool bDevisedKeyboard,
	FInputAxisKeyMapping& OutMapping)
{
	TArray<FInputAxisKeyMapping> tarrMappings;
	UPxcBlueprintLibrary::Key_GetAllAxisMappingByName(AxisName, bPositiveDir, tarrMappings);
	if (tarrMappings.Num() > 0)
		return UPxcBlueprintLibrary::Key_GetIndexedDevisedAxMapping(tarrMappings, bDevisedKeyboard, 0, OutMapping);
	return false;
}