// Fill out your copyright notice in the Description page of Project Settings.


#include "SignatureStructs.h"

uint32 GetTypeHash(const FPointerModel& Model)
{
	return GetTypeHash(Model.lUid);
}