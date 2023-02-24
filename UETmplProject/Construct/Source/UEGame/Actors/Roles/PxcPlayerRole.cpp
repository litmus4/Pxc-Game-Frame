// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/Roles/PxcPlayerRole.h"
#include "Actors/Characters/PxcPlayerCharacter.h"

APxcPlayerCharacter* APxcPlayerRole::GetThePlayerCharacter()
{
	return m_pPlayerCharacter;
}

void APxcPlayerRole::SetThePlayerCharacter(APxcPlayerCharacter* pCharacter)
{
	check(IsValid(pCharacter));
	m_pPlayerCharacter = pCharacter;
}