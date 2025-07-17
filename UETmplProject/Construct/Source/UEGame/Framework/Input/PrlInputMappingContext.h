// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputMappingContext.h"
#include "PrlInputMappingContext.generated.h"

class UEnhancedInputLocalPlayerSubsystem;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class UEGAME_API UPrlInputMappingContext : public UInputMappingContext
{
	GENERATED_BODY()
	friend class APxcPlayerController;
	
public:
	UFUNCTION(BlueprintCallable, Category = Settings)
	bool SetSubsystemFromController(APlayerController* pPlayerController);

	UFUNCTION(BlueprintCallable, Category = Settings)
	void AddActionMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps = true);

	UFUNCTION(BlueprintPure, Category = Settings)
	void GetActionMappingByName(const FName ActionName, TArray<FEnhancedActionKeyMapping>& OutMappings) const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void RemoveActionMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps = true);

    UFUNCTION(BlueprintCallable, Category = Settings)
	void AddAxisMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps = true);

	UFUNCTION(BlueprintPure, Category = Settings)
	void GetAxisMappingByName(const FName AxisName, TArray<FEnhancedActionKeyMapping>& OutMappings) const;

	UFUNCTION(BlueprintCallable, Category = Settings)
	void RemoveAxisMapping(const FEnhancedActionKeyMapping& KeyMapping, bool bForceRebuildKeymaps = true);

	//*≤‚ ‘¡Ÿ ±
	UFUNCTION(BlueprintCallable, Category = Test)
	void TestMappingEqual(const FEnhancedActionKeyMapping& KeyMapping3);
	//*/

private:
	UPROPERTY(Transient)
	ULocalPlayer* m_pLocalPlayer = nullptr;

	UPROPERTY(Transient)
	UEnhancedInputLocalPlayerSubsystem* m_pSubsystem = nullptr;
};
