// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcNativeLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"
#include "Framework/Structs/InputStructs.h"

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

bool FPxcNativeLibrary::EI_IsMappingEqual(const FEnhancedActionKeyMapping& Mapping1, const FEnhancedActionKeyMapping& Mapping2)
{
	if (Mapping1.Action == Mapping2.Action && Mapping1.Key == Mapping2.Key)
	{
		if (Mapping1.Triggers.Num() == Mapping2.Triggers.Num())
		{
			if (Mapping1.Triggers.IsEmpty())
				return true;
			else if (Mapping1.Triggers.Num() == 1)
			{
				UInputTriggerPrlChordAction* pTriggerPrlChord1 = Cast<UInputTriggerPrlChordAction>(Mapping1.Triggers[0]);
				UInputTriggerPrlChordAction* pTriggerPrlChord2 = Cast<UInputTriggerPrlChordAction>(Mapping2.Triggers[0]);
				if (pTriggerPrlChord1 && pTriggerPrlChord2 &&
					pTriggerPrlChord1->CombinedChordAction == pTriggerPrlChord2->CombinedChordAction)
					return true;
				else
				{
					UInputTriggerChordAction* pTriggerChord1 = Cast<UInputTriggerChordAction>(Mapping1.Triggers[0]);
					UInputTriggerChordAction* pTriggerChord2 = Cast<UInputTriggerChordAction>(Mapping2.Triggers[0]);
					if (pTriggerChord1 && pTriggerChord2)
					{
						if (pTriggerChord1->ChordAction == pTriggerChord2->ChordAction)
							return true;
					}
					else
						checkNoEntry();
				}
			}
			else
				checkNoEntry();
		}
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