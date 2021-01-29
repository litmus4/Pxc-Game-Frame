// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Framework/Structs/ScatteredStructs.h"
#include "../Utilities/UtilityStructs.h"
#include <functional>
#include "PxcBlueprintLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPxcBPLib, Display, All);

/**
 * 
 */
UCLASS()
class UE4GAME_API UPxcBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	//TODOJK 在ASC的OnGameplayEffectAppliedDelegateToSelf回调内部调用
	UFUNCTION(BlueprintCallable, Category = "GameplayEffects|Extention")
	static FActiveGameplayEffectHandle ApplyExtentionGESpecWithReturn(UAbilitySystemComponent* pASC, const FGameplayEffectSpecHandle& SpecHandle,
		FActiveGameplayEffectHandle StackedHandle, TMap<FString, float>& tmapRet);

	UFUNCTION(BlueprintCallable, Category = "GameplayEffects|Extention")
	static void DiscardExtentionGEByStackedHandle(UAbilitySystemComponent* pASC, FActiveGameplayEffectHandle StackedHandle);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static int32 PushIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static void PopIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, int32 iUid);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static int32 PushIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static void PopIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, int32 iUid);

	//=====================

	UFUNCTION(BlueprintPure, Category = "Input|Key")
	static FName Key_GetName(const FKey& Key);

	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static void Key_GetAllActionMappingByName(const FName& ActionName,
		TArray<FInputActionKeyMapping>& tarrOutMappings, TArray<FGamepadCombineMapping>& tarrOutGamepadCombMappings);

	//ReturnValue >0:Mapping <0:GamepadCombMapping ==0:没找到
	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static int32 Key_GetFirstAdaptiveAcMapping(const TArray<FInputActionKeyMapping>& tarrMappings, const TArray<FGamepadCombineMapping>& tarrGamepadCombMappings,
		FInputActionKeyMapping& OutMapping, FGamepadCombineMapping& OutGamepadCombMapping);

	//ReturnValue >0:Mapping <0:GamepadCombMapping ==0:没找到
	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static int32 Key_GetIndexedDevisedAcMapping(const TArray<FInputActionKeyMapping>& tarrMappings, const TArray<FGamepadCombineMapping>& tarrGamepadCombMappings,
		bool bDevisedKeyboard, int32 iIndex, bool bGamepadCombBoth, FInputActionKeyMapping& OutMapping, FGamepadCombineMapping& OutGamepadCombMapping);

	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static void Key_GetAllAxisMappingByName(const FName& AxisName, bool bPositiveDir, TArray<FInputAxisKeyMapping>& tarrOutMappings);

	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static bool Key_GetFirstAdaptiveAxMapping(const TArray<FInputAxisKeyMapping>& tarrMappings, FInputAxisKeyMapping& OutMapping);

	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static bool Key_GetIndexedDevisedAxMapping(const TArray<FInputAxisKeyMapping>& tarrMappings,
		bool bDevisedKeyboard, int32 iIndex, FInputAxisKeyMapping& OutMapping);

	UFUNCTION(BlueprintPure, Category = "Input|Key")
	static FPxcInputMapping Key_MakeInputMapping(const FName& ActionName);

	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static UMaterialInstance* Key_ParseIconsFromInputMapping(const FPxcInputMapping& InputMapping, int32 iModifierNum,
		TArray<UMaterialInstance*>& tarrOutModifierIcons);

public:
	template<class T>
	static void ForEachUDTRowValue(UDataTable* pUDT, std::function<void(const T*)>&& fnOnEach)
	{
		static_assert(TPointerIsConvertibleFromTo<T, FTableRowBase>::Value,
			"UPxcBlueprintLibrary ForEachRowValueInUDT static_assert");
		if (ensureAlways(IsValid(pUDT) && pUDT->GetRowStruct() && pUDT->GetRowStruct()->IsChildOf(T::StaticStruct())))
		{
			TMap<FName, uint8*>::TConstIterator Iter(pUDT->GetRowMap().CreateConstIterator());
			for (; Iter; ++Iter)
				fnOnEach((T*)Iter.Value());
		}
		else
		{
			UE_LOG(LogPxcBPLib, Error, TEXT("ForEachRowValueInUDT: UDT is invalid!"));
			//TODOJK 使用LogCenter
		}
	}
};

#define LOAD_UDT_TO_TMAP(UDTPtr, RowClass, Container, IDMbr) UPxcBlueprintLibrary::ForEachUDTRowValue<RowClass>(\
	UDTPtr, [this](const RowClass* pRow) {\
	check(!Container.Find(pRow->IDMbr));\
	Container.Add(pRow->IDMbr, *pRow);\
})

#define LOAD_UDT_TO_MAP(UDTPtr, RowClass, Container, IDClass, IDMbr) UPxcBlueprintLibrary::ForEachUDTRowValue<RowClass>(\
	UDTPtr, [this](const RowClass* pRow) {\
	check(Container.find(pRow->IDMbr) == Container.end());\
	Container.insert(std::pair<IDClass, RowClass>pRow->IDMbr, *pRow);\
})