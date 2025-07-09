// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/Structs/InputStructs.h"
#include "Misc/DataValidation.h"
#include "EnhancedPlayerInput.h"

#if WITH_EDITOR
#define LOCTEXT_PRL(InKey, InTextLiteral) FText::AsLocalizable_Advanced(TEXT(""), TEXT(InKey), TEXT(InTextLiteral))
EDataValidationResult UInputTriggerPrlChordAction::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

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