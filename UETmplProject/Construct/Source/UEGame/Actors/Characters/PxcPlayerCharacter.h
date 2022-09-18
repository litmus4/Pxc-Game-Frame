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

UENUM(BlueprintType)
enum class EEndMoveFootType : uint8
{
	Any = 0,
	Left,
	Right
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndMoveStartDelegate);

class UCameraComponent;
class USpringArmComponent;
class UAnimInstance;

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

	UFUNCTION(BlueprintCallable, Category = Animation)
	void CheckMachineStateEnd(UAnimInstance* pABP, FName MachineName, EMotionState eMotionState, float fTimeRemaining);

	UFUNCTION(BlueprintCallable, Category = Animation)
	void RunLocoMotionEndMoveInput();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraComponent* m_pCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USpringArmComponent* m_pSpringArmComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	bool m_bRootMotion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	EAnimBPType m_eAnimBPType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	UCurveFloat* m_pStartMoveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	UCurveFloat* m_pEndMoveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FName m_LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FName m_RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	float m_fFootDownLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	TArray<TEnumAsByte<EObjectTypeQuery>> m_tarrFootTraceTypes;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	FVector m_vFocus;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	EMotionState m_eMotionState;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	float m_fStartMoveMaxTime;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	float m_fEndMoveMaxTime;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	EEndMoveFootType m_eFootType;

	UPROPERTY(BlueprintAssignable, Category = Animation)
	FEndMoveStartDelegate DeleEndMoveStarted;

public:
	virtual void Tick(float fDeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* pPlayerInputComponent) override;

protected:
	void OnMoveForward(float fValue);
	void OnMoveRight(float fValue);
	void OnMotionStateEnd(EMotionState eMotionState);
	void OnEndMoveStarted();

	void ResetLocoMotionStartMoveTime();
	void ResetLocoMotionEndMoveTime();

	FVector2D m_v2Axis;
	FVector2D m_v2LastAxis;
	float m_fStartMoveTime;
	float m_fEndMoveTime;
	float m_fStartMoveStamp;
	float m_fEndMoveStamp;
	bool m_bEndMoveStarted;
};