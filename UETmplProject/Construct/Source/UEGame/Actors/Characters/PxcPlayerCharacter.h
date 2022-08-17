// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/PxcGraphCharacter.h"
#include "PxcPlayerCharacter.generated.h"

UENUM(BlueprintType)
enum class EAnimBPType : uint8
{
	DirectionTurned,
	ControllerTurned
};

UENUM(BlueprintType)
enum class EMotionState : uint8
{
	Idle = 0,
	StartMove,
	Move,
	EndMove
};

class UCameraComponent;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class UEGAME_API APxcPlayerCharacter : public APxcGraphCharacter
{
	GENERATED_BODY()
	
public:
	APxcPlayerCharacter();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraComponent* m_pCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USpringArmComponent* m_pSpringArmComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	bool m_bRootMotion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	EAnimBPType m_eAnimBPType;

	UPROPERTY(BlueprintReadOnly)
	FVector m_vTurn;

	UPROPERTY(BlueprintReadWrite)
	EMotionState m_eMotionState;

public:
	virtual void Tick(float fDeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* pPlayerInputComponent) override;

protected:
	void OnMoveForward(float fValue);
	void OnMoveRight(float fValue);

	FVector2D m_v2Axis;
	FVector2D m_v2LastAxis;
};