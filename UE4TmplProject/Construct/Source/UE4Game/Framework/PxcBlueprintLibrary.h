// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystemComponent.h"
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