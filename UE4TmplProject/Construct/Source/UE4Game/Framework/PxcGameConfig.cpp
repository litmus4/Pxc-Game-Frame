// Fill out your copyright notice in the Description page of Project Settings.


#include "PxcGameConfig.h"

void UPxcGameConfig::AddGamepadCombineMapping(const FGamepadCombineMapping& CombineMapping)
{
	tarrGamepadCombineMappings.AddUnique(CombineMapping);
	bGamepadCombineDirty = true;
}

void UPxcGameConfig::RemoveGamepadCombineMapping(const FGamepadCombineMapping& CombineMapping)
{
	for (int32 i = tarrGamepadCombineMappings.Num() - 1; i >= 0; --i)
	{
		if (tarrGamepadCombineMappings[i] == CombineMapping)
			tarrGamepadCombineMappings.RemoveAt(i);
	}
	bGamepadCombineDirty = true;
}

const TMap<FName, TArray<FGamepadCombineMapping>>& UPxcGameConfig::GetGamepadCombineMappings() const
{
	static TMap<FName, TArray<FGamepadCombineMapping>> tmapRet;
	if (tmapRet.Num() == 0 || bGamepadCombineDirty)
	{
		tmapRet.Empty();
		for (const FGamepadCombineMapping& CombineMapping : tarrGamepadCombineMappings)
		{
			TArray<FGamepadCombineMapping>& tarrCombMappingList = tmapRet.FindOrAdd(CombineMapping.ActionName);
			tarrCombMappingList.Add(CombineMapping);
		}
		if (bGamepadCombineDirty)
			const_cast<UPxcGameConfig*>(this)->bGamepadCombineDirty = false;
	}
	return tmapRet;
}