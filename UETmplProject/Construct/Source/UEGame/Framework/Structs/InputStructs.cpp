// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Structs/InputStructs.h"
#include "Misc/DataValidation.h"
#include "EnhancedPlayerInput.h"

#if WITH_EDITOR
#define LOCTEXT_PRL(InKey, InTextLiteral) FText::AsLocalizable_Advanced(TEXT(""), TEXT(InKey), TEXT(InTextLiteral))
EDataValidationResult UInputTriggerPrlChordAction::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(UObject::IsDataValid(Context), EDataValidationResult::Valid);

	// You can't evaluate the combo if there are no combo steps!
	if (!CombinedChordAction)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(LOCTEXT_PRL("NullChordedAction", "A valid action is required for the Chorded Action input trigger!"));
	}

	return Result;
}
#endif

ETriggerState UInputTriggerPrlChordAction::UpdateState_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue ModifiedValue, float DeltaTime)
{
	ChordAction = CombinedChordAction;
	return Super::UpdateState_Implementation(PlayerInput, ModifiedValue, DeltaTime);
}

bool FPrlActionMappingConfig::operator==(const FPrlActionMappingConfig& Other) const
{
	return (ActionName == Other.ActionName &&
		Key == Other.Key &&
		ChordActionName == Other.ChordActionName);
}

bool FPrlActionMappingConfig::operator<(const FPrlActionMappingConfig& Other) const
{
	bool bRet = false;
	if (ActionName.LexicalLess(Other.ActionName))
		bRet = true;
	else if (ActionName == Other.ActionName)
		bRet = (Key < Other.Key);
	return bRet;
}

bool FPrlAxisMappingConfig::operator==(const FPrlAxisMappingConfig& Other) const
{
	return (AxisName == Other.AxisName &&
		Key == Other.Key &&
		bNegative == Other.bNegative);
}

bool FPrlAxisMappingConfig::operator<(const FPrlAxisMappingConfig& Other) const
{
	bool bRet = false;
	if (AxisName.LexicalLess(Other.AxisName))
		bRet = true;
	else if (AxisName == Other.AxisName)
	{
		if (Key < Other.Key)
			bRet = true;
		else if (Key == Other.Key)
			bRet = (bNegative == false && Other.bNegative == true);
	}
	return bRet;
}