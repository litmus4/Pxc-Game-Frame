// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "PxcAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName)\
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

#define CANREVISE_PAIR(AttrName) {Get##AttrName##Attribute(), Get##AttrName}

class UGameplayEffect;

/**
 * 
 */
UCLASS()
class UEGAME_API UPxcAttributeSet : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& fNewValue) override;
	//virtual bool PreGameplayEffectExecute(struct FGameplayEffectModCallbackData& Data) override;
	virtual void PreGameplayEffectContModify(const UGameplayEffect* pEffect, int32 iModIndex, float& fNewValue, float& fDiff);
	virtual void PreRevisedAttributeChange(const FGameplayAttribute& Attribute, float& NewValue, float fDiff);

protected:
	virtual bool CanAttrbuteRevisedByGE(const FGameplayAttribute& Attribute, float* pOutValue = nullptr);
	void TryTriggerPreGEContModify(const FGameplayAttribute& Attribute, float& fNewValue, float& fDiff);
};
