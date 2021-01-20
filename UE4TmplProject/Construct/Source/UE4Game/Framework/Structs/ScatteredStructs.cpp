// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Structs/ScatteredStructs.h"

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
	//UISSBlueprintLibrary::Key_GetAllActionMappingByName(ActionName, tarrMappings, tarrGamepadCombMappings);
	//if (Mappings.Num() == 0 && GamepadCombMappings.Num() == 0)
	//	return;

	//FInputActionKeyMapping FirstMapping;
	//FGamepadCombineMapping FirstCombineMapping;
	//int32 Result = 0;
	//if (DevisedKeyboard)
	//{
	//	Result = UISSBlueprintLibrary::Key_GetIndexedDevisedAcMapping(Mappings, GamepadCombMappings,
	//		*DevisedKeyboard, FMath::Max(Index, 0), false, FirstMapping, FirstCombineMapping);
	//}
	//else
	//	Result = UISSBlueprintLibrary::Key_GetFirstAdaptiveAcMapping(Mappings, GamepadCombMappings, FirstMapping, FirstCombineMapping);
	//if (!Result) return;

	//KeyName = (Result > 0 ? FirstMapping.Key.GetFName() : FirstCombineMapping.MainKey.GetFName());

	//if (Result > 0)
	//{
	//	if (FirstMapping.bShift) ModifierCode |= 1;
	//	if (FirstMapping.bCtrl) ModifierCode |= 1 << 1;
	//	if (FirstMapping.bAlt) ModifierCode |= 1 << 2;
	//	if (FirstMapping.bCmd) ModifierCode |= 1 << 3;
	//}
	//else
	//	ModifierCode = 1 << ((int32)FirstCombineMapping.ModifierKey + 4);
}