// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "PxcLogicRole.generated.h"

class UPxcAbilitySystemComponent;

UCLASS()
class UEGAME_API APxcLogicRole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APxcLogicRole();

	UFUNCTION(BlueprintPure, Category = GameplayAbility)
	virtual UPxcAbilitySystemComponent* GetAbilitySystemComponent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnGameplayEffectApplied(UAbilitySystemComponent* pASC, const FGameplayEffectSpec& Spec, FActiveGameplayEffectHandle Handle);

	UFUNCTION()
	void OnGameplayEffectRemoved(const FActiveGameplayEffect& ActiveEffect);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UPxcAbilitySystemComponent* m_pPASC;

public:	
	// Called every frame
	virtual void Tick(float fDeltaTime) override;

};
