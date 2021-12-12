// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Structs/ScatteredStructs.h"
#include "Framework/PxcBlueprintLibrary.h"

bool FGamepadCombineMapping::operator==(const FGamepadCombineMapping& Other) const
{
	return (ActionName == Other.ActionName &&
		MainKey == Other.MainKey &&
		eModifierKey == Other.eModifierKey);
}

FPxcInputMapping::FPxcInputMapping(const FName& xActionName)
	: ActionName(xActionName), uModifierCode(0)
{
	InitMapping();
}

FPxcInputMapping::FPxcInputMapping(const FName& xActionName, bool bDevisedKeyboard, int32 iIndex)
	: ActionName(xActionName), uModifierCode(0)
{
	InitMapping(&bDevisedKeyboard, iIndex);
}

void FPxcInputMapping::InitMapping(bool* pbDevisedKeyboard, int32 iIndex)
{
	TArray<FInputActionKeyMapping> tarrMappings;
	TArray<FGamepadCombineMapping> tarrGamepadCombMappings;
	UPxcBlueprintLibrary::Key_GetAllActionMappingByName(ActionName, tarrMappings, tarrGamepadCombMappings);
	if (tarrMappings.Num() == 0 && tarrGamepadCombMappings.Num() == 0)
		return;

	FInputActionKeyMapping FirstMapping;
	FGamepadCombineMapping FirstCombineMapping;
	int32 iResult = 0;
	if (pbDevisedKeyboard)
	{
		iResult = UPxcBlueprintLibrary::Key_GetIndexedDevisedAcMapping(tarrMappings, tarrGamepadCombMappings,
			*pbDevisedKeyboard, FMath::Max(iIndex, 0), false, FirstMapping, FirstCombineMapping);
	}
	else
		iResult = UPxcBlueprintLibrary::Key_GetFirstAdaptiveAcMapping(tarrMappings, tarrGamepadCombMappings, FirstMapping, FirstCombineMapping);
	if (!iResult) return;

	KeyName = (iResult > 0 ? FirstMapping.Key.GetFName() : FirstCombineMapping.MainKey.GetFName());

	if (iResult > 0)
	{
		if (FirstMapping.bShift) uModifierCode |= 1;
		if (FirstMapping.bCtrl) uModifierCode |= 1 << 1;
		if (FirstMapping.bAlt) uModifierCode |= 1 << 2;
		if (FirstMapping.bCmd) uModifierCode |= 1 << 3;
	}
	else
		uModifierCode = 1 << ((int32)FirstCombineMapping.eModifierKey + 4);
}

bool FPxcInputMapping::IsSame(const FPxcInputMapping& Other)
{
	return (KeyName == Other.KeyName && uModifierCode == Other.uModifierCode);
}

bool FPxcInputMapping::IsSame(const FPxcAxputMapping& Other)
{
	return (uModifierCode != 0 ? false : KeyName == Other.KeyName);
}

FPxcAxputMapping::FPxcAxputMapping(const FName& xAxisName, bool bPositive)
	: AxisName(xAxisName), bPositiveDir(bPositive), iFloatPositive(0)
{
	InitMapping();
}

FPxcAxputMapping::FPxcAxputMapping(const FName& xAxisName, bool bPositive, bool bDevisedKeyboard, int32 iIndex)
	: AxisName(xAxisName), bPositiveDir(bPositive), iFloatPositive(0)
{
	InitMapping(&bDevisedKeyboard, iIndex);
}

void FPxcAxputMapping::InitMapping(bool* pbDevisedKeyboard, int32 iIndex)
{
	TArray<FInputAxisKeyMapping> tarrMappings;
	UPxcBlueprintLibrary::Key_GetAllAxisMappingByName(AxisName, bPositiveDir, tarrMappings);
	if (tarrMappings.Num() == 0) return;

	FInputAxisKeyMapping FirstMapping;
	bool bResult = false;
	if (pbDevisedKeyboard)
		bResult = UPxcBlueprintLibrary::Key_GetIndexedDevisedAxMapping(tarrMappings, *pbDevisedKeyboard, FMath::Max(iIndex, 0), FirstMapping);
	else
		bResult = UPxcBlueprintLibrary::Key_GetFirstAdaptiveAxMapping(tarrMappings, FirstMapping);

	if (bResult)
	{
		KeyName = FirstMapping.Key.GetFName();
		if (FirstMapping.Key.IsAxis1D())
			iFloatPositive = (bPositiveDir == (FirstMapping.Scale >= 0) ? 1 : -1);
	}
}

bool FPxcAxputMapping::IsSame(const FPxcInputMapping& Other)
{
	return (Other.uModifierCode != 0 ? false : KeyName == Other.KeyName);
}