// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "DataRowBridge.generated.h"

class CCSVTableOperator;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDataRowBridge : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class UEGAME_API IDataRowBridge
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(CallInEditor, Category = DataRow)
	virtual bool LoadDataRow(int iID = -1);

	UFUNCTION(CallInEditor, Category = DataRow)
	virtual bool SaveDataRow();

protected:
	CCSVTableOperator* m_pTabop;
};
