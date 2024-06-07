// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcNativeLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Actor.h"

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