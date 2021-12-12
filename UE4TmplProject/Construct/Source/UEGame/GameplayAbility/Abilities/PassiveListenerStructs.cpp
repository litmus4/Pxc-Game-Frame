// Fill out your copyright notice in the Description page of Project Settings.


#include "PassiveLIstenerStructs.h"

float FPassiveAttackExListener::Process(float fInValue, float fBaseValue)
{
	if (iProcStep == 0)
	{
		fProcValue = fBaseValue * fAttackMulti + fAttackAdd;
		iProcStep++;
		return fInValue;
	}
	iProcStep = 0;
	return fInValue + fProcValue;
}

void FPassiveAttackExListener::Restart()
{
	iProcStep = 0;
}