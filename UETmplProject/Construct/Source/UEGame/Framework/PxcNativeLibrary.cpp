// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcNativeLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Framework/Structs/InputStructs.h"
#include "Framework/PxcGameConfig.h"

void FPxcNativeLibrary::GetAllActorsInSubLevel(UWorld* pWorld, ULevelStreaming* pLevelStreaming, TSubclassOf<AActor> Class, TArray<AActor*>& tarrOutActors,
	std::function<bool(AActor*)> fnFilter)
{
	if (!pWorld || !pLevelStreaming || !Class.Get())
		return;

	TArray<AActor*> tarrInActors;
	UGameplayStatics::GetAllActorsOfClass(pWorld, Class, tarrInActors);
	for (AActor* pActor : tarrInActors)
	{
		if (IsValid(pActor) && pActor->GetLevel() == pLevelStreaming->GetLoadedLevel())
		{
			if (fnFilter(pActor))
				tarrOutActors.Add(pActor);
		}
	}
}

uint8 FPxcNativeLibrary::EI_GetModifierCodeFromTriggers(const TArray<UInputTrigger*>& tarrTriggers)
{
	uint8 uRet = 0;
	const TArray<TSoftObjectPtr<UInputAction>>& tarrPrlChordActions = GetDefault<UPxcGameConfig>()->tarrPrlChordActions;
	/*²âÊÔÁÙÊ±
	if (tarrPrlChordActions.Num() > 0 && tarrTriggers.Num() > 0)
	{
		const TSoftObjectPtr<UInputAction>& pTestPrlChordAction = tarrPrlChordActions[0];
		UInputTriggerChordAction* pTestTriggerChord = Cast<UInputTriggerChordAction>(tarrTriggers[0]);
		if (pTestTriggerChord)
		{
			bool bTest = (pTestPrlChordAction == pTestTriggerChord->ChordAction.Get());
			int32 iii = 0;
		}
	}
	//*/
	for (UInputTrigger* pTrigger : tarrTriggers)
	{
		if (!IsValid(pTrigger)) continue;

		int32 iChordIndex = -1;
		UInputTriggerPrlChordAction* pTriggerPrlChord = Cast<UInputTriggerPrlChordAction>(pTrigger);
		if (pTriggerPrlChord)
		{
			UInputAction* pNoConstCCA = const_cast<UInputAction*>(pTriggerPrlChord->CombinedChordAction.Get());
			if (tarrPrlChordActions.Find(pNoConstCCA, iChordIndex))
				uRet |= 1 << iChordIndex;
			else
				checkNoEntry();
		}
		else
		{
			UInputTriggerChordAction* pTriggerChord = Cast<UInputTriggerChordAction>(pTrigger);
			if (pTriggerChord)
			{
				UInputAction* pNoConstCA = const_cast<UInputAction*>(pTriggerChord->ChordAction.Get());
				if (tarrPrlChordActions.Find(pNoConstCA, iChordIndex))
					uRet |= 1 << iChordIndex;
				else
					checkNoEntry();
			}
		}
	}
	return uRet;
}

bool FPxcNativeLibrary::EI_IsMappingEqual(const FEnhancedActionKeyMapping& Mapping1, const FEnhancedActionKeyMapping& Mapping2)
{
	if (Mapping1.Action == Mapping2.Action && Mapping1.Key == Mapping2.Key)
	{
		uint8 uChordCode1 = EI_GetModifierCodeFromTriggers(Mapping1.Triggers);
		uint8 uChordCode2 = EI_GetModifierCodeFromTriggers(Mapping2.Triggers);
		return (uChordCode1 == uChordCode2);
	}
	return false;
}

int32 FPxcNativeLibrary::EI_AddUniqueMapping(TArray<FEnhancedActionKeyMapping>& tarrMappings, const FEnhancedActionKeyMapping& Mapping)
{
	for (int32 i = 0; i < tarrMappings.Num(); ++i)
	{
		if (EI_IsMappingEqual(tarrMappings[i], Mapping))
			return i;
	}
	return tarrMappings.Add(Mapping);
}