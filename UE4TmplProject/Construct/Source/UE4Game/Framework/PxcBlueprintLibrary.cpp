// Fill out your copyright notice in the Description page of Project Settings.


#include "Framework/PxcBlueprintLibrary.h"

int32 UPxcBlueprintLibrary::PushIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, float fValue)
{
	return Stack.PushValue(fValue);
}

void UPxcBlueprintLibrary::PopIdentifiedLessWhenGE(UPARAM(ref) FIdentifiedLessStack& Stack, int32 iUid)
{
	Stack.PopValue(iUid);
}

int32 UPxcBlueprintLibrary::PushIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, float fValue)
{
	return Stack.PushValue(fValue);
}

void UPxcBlueprintLibrary::PopIdentifiedGreaterWhenGE(UPARAM(ref) FIdentifiedGreaterStack& Stack, int32 iUid)
{
	Stack.PopValue(iUid);
}