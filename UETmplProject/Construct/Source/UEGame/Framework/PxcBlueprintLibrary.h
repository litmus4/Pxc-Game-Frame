// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
#include "Structs/VirtualGroupStructs.h"
#include "Structs/SignatureStructs.h"
#include "Structs/ScatteredStructs.h"
#include "../Utilities/UtilityStructs.h"
#include <functional>
#include "PxcBlueprintLibrary.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPxcBPLib, Display, All);

class UVirtualGroupMgr;

/**
 * 
 */
UCLASS()
class UEGAME_API UPxcBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay|VirtualGroup", meta = (WorldContext = "pWCO"))
	static void SetVirtGrpRTDFeatureBack(UObject* pWCO, UPARAM(ref) FVirtGrpRTDFeature& Feature);

	UFUNCTION(BlueprintCallable, Category = "GameplayAbility|PassiveListening")
	static int64 MakePassiveAttackExListenerSignature(float fAttackAdd, float fAttackMulti, FSharedSignature& OutSig);

	UFUNCTION(BlueprintCallable, Category = "GameplayEffects|Extention")
	static FActiveGameplayEffectHandle ApplyExtentionGESpecWithReturn(UAbilitySystemComponent* pASC, const FGameplayEffectSpecHandle& SpecHandle,
		FActiveGameplayEffectHandle StackedHandle, TMap<FString, float>& tmapRet);

	UFUNCTION(BlueprintCallable, Category = "GameplayEffects|Extention")
	static void DiscardExtentionGEByStackedHandle(UAbilitySystemComponent* pASC, FActiveGameplayEffectHandle StackedHandle);

	UFUNCTION(BlueprintPure, Category = "Utility|GEExtention")
	static void CutGEExtentionVariable(float fVariable, TArray<int32> tarrDecSplits, TArray<int32>& tarrOutValues);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static int32 PushIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static void PopIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, int32 iUid);

	UFUNCTION(BlueprintPure, Category = "Utility|IdentifiedStack")
	static bool GetIdentifiedLessTop(const FIdentifiedLessStack& Stack, float& fOutValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static int32 PushIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, float fValue);

	UFUNCTION(BlueprintCallable, Category = "Utility|IdentifiedStack")
	static void PopIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, int32 iUid);

	UFUNCTION(BlueprintPure, Category = "Utility|IdentifiedStack")
	static bool GetIdentifiedGreaterTop(const FIdentifiedGreaterStack& Stack, float& fOutValue);

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
	static UMaterialInstance* Key_ParseIconFromInputMapping(const FPxcInputMapping& InputMapping, int32 iModifierNum,
		TArray<UMaterialInstance*>& tarrOutModifierIcons);

	UFUNCTION(BlueprintPure, Category = "Input|Key")
	static FPxcAxputMapping Key_MakeAxputMapping(const FName& AxisName, bool bPositiveDir);

	UFUNCTION(BlueprintCallable, Category = "Input|Key")
	static UMaterialInstance* Key_ParseIconFromAxputMapping(const FPxcAxputMapping& AxputMapping);

public:
	template<class T>
	static void ForEachUDTRowValue(UDataTable* pUDT, std::function<void(const T*)>&& fnOnEach)
	{
		static_assert(TPointerIsConvertibleFromTo<T, FTableRowBase>::Value,
			"UPxcBlueprintLibrary ForEachUDTRowValue static_assert");
		if (ensureAlways(IsValid(pUDT) && pUDT->GetRowStruct() && pUDT->GetRowStruct()->IsChildOf(T::StaticStruct())))
		{
			TMap<FName, uint8*>::TConstIterator Iter(pUDT->GetRowMap().CreateConstIterator());
			for (; Iter; ++Iter)
				fnOnEach((T*)Iter.Value());
		}
		else
		{
			UE_LOG(LogPxcBPLib, Error, TEXT("ForEachUDTRowValue: UDT is invalid!"));
			//TODOJK 使用LogCenter
		}
	}

public:
	UFUNCTION(BlueprintPure, Category = "Utility|Math")
	static float Math_MagClamp(float fValue, float fMin, float fMax,
		bool bMidAdsorption = false, bool bMidToMax = true, float fPrecision = 0.00001f);
};

#define LOAD_UDT_TO_TMAP(UDTPtr, RowClass, Container, IDMbr) UPxcBlueprintLibrary::ForEachUDTRowValue<RowClass>(\
	UDTPtr, [this](const RowClass* pRow) {\
	check(!Container.Find(pRow->IDMbr));\
	Container.Add(pRow->IDMbr, *pRow);\
})

#define LOAD_UDT_TO_MAP(UDTPtr, RowClass, Container, IDClass, IDMbr) UPxcBlueprintLibrary::ForEachUDTRowValue<RowClass>(\
	UDTPtr, [this](const RowClass* pRow) {\
	check(Container.find(pRow->IDMbr) == Container.end());\
	Container.insert(std::pair<IDClass, RowClass>(pRow->IDMbr, *pRow));\
})