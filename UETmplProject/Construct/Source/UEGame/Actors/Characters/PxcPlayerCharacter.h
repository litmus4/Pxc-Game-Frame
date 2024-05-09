// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Characters/PxcGraphCharacter.h"
#include "PrivateDefinitions/CharacterDef.h"
#include <unordered_map>
#include "PxcPlayerCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEndMoveStartDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerUniversalEventDelegate, class APxcPlayerCharacter*, pPlayer);
DECLARE_DYNAMIC_DELEGATE_OneParam(FPlayerUniversalOneDelegate, APxcPlayerCharacter*, pPlayer);

class UCameraComponent;
class USpringArmComponent;
class UAnimInstance;
class UPxcCharacterTrajectoryComponent;
struct FPoseSearchQueryTrajectory;

/**
 * 
 */
UCLASS()
class UEGAME_API APxcPlayerCharacter : public APxcGraphCharacter
{
	GENERATED_BODY()
	
public:
	APxcPlayerCharacter();

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void BindUniversalEventByType(EPlayerUniEventType eType, FPlayerUniversalOneDelegate DeleCall);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void UnbindUniversalEventsFromType(EPlayerUniEventType eType, UObject* pObject);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void UnbindAllUniversalEventsFromType(EPlayerUniEventType eType);

	UFUNCTION(BlueprintCallable, Category = Gameplay)
	void CallUniversalEventByType(EPlayerUniEventType eType);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable, Category = Animation)
	void CheckMachineStateEnd(UAnimInstance* pABP, FName MachineName, EMotionState eMotionState, float fTimeRemaining);

	UFUNCTION(BlueprintPure, Category = Animation)
	float GetMachineStateLength(UAnimInstance* pABP, FName MachineName, EMotionState eMotionState);

	UFUNCTION(BlueprintCallable, Category = Animation)
	void RunLocoMotionEndMoveInput();

	UFUNCTION(BlueprintCallable, Category = Animation)
	bool GetMotionTrajectory(FPoseSearchQueryTrajectory& OutTrajectory);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UCameraComponent* m_pCameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	USpringArmComponent* m_pSpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UPxcCharacterTrajectoryComponent* m_pTrajectoryComp;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	bool m_bRootMotion;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	EAnimBPType m_eAnimBPType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	FName m_AnimMachineName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	UCurveFloat* m_pStartMoveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	UCurveFloat* m_pEndMoveCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FName m_LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	FName m_RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Animation)
	double m_dFootDownLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Animation)
	TArray<TEnumAsByte<EObjectTypeQuery>> m_tarrFootTraceTypes;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	FVector m_vFocus;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	EMotionState m_eMotionState;

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	float m_fStartMoveMaxTime;//曲线默认时长

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	float m_fEndMoveMaxTime;//曲线默认时长

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	float m_fStartMoveLength;//状态实际时长

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	float m_fEndMoveLength;//状态实际时长

	UPROPERTY(BlueprintReadOnly, Category = Animation)
	EEndMoveFootType m_eFootType;

	UPROPERTY(BlueprintAssignable, Category = Animation)
	FEndMoveStartDelegate DeleEndMoveStarted;

public:
	virtual void PossessedBy(AController* pNewController) override;
	virtual void Tick(float fDeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* pPlayerInputComponent) override;

protected:
	void OnMoveForward(float fValue);
	void OnMoveRight(float fValue);
	void OnMotionStateEnd(EMotionState eMotionState);
	void OnEndMoveStarted();

	void CheckAndBeginStartToEndBridge();
	bool BuildStartToEndBridge(float& fValue);
	void CheckAndBeginEndToStartBridge();
	bool BuildEndToStartBridge(float& fValue);

	void ResetLocoMotionStartMoveTime();
	void ResetLocoMotionEndMoveTime();

	std::unordered_map<EPlayerUniEventType, FPlayerUniversalEventDelegate> m_mapUniversalEvents;

	FVector2D m_v2Axis;
	FVector2D m_v2LastAxis;

	FPoseSearchQueryTrajectory* m_pTrajectory;

	float m_fStartMoveTime;
	float m_fEndMoveTime;
	float m_fStartMoveStamp;
	float m_fEndMoveStamp;

	float m_fStartToEndValue;
	float m_fEndToStartValue;

	bool m_bEndMoveStarted;
};