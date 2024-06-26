// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LevelPreparedActor.h"
#include "Kismet/GameplayStatics.h"
#include "VT/RuntimeVirtualTextureVolume.h"
#include "Engine/Texture2D.h"
#include "LandscapeProxy.h"

// Sets default values
ALevelPreparedActor::ALevelPreparedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALevelPreparedActor::BeginPlay()
{
	Super::BeginPlay();
}

UTexture2D* ALevelPreparedActor::CreateMaterialRVTVArrayTexture(int32 iSizeX, int32 iSizeY)
{
	TArray<AActor*> tarrVolumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ARuntimeVirtualTextureVolume::StaticClass(), tarrVolumes);
	//FLAGJK_NOW
	return nullptr;
}

void ALevelPreparedActor::PreRegisterAllComponents()
{
	Super::PreRegisterAllComponents();

	InitLevelEffects();
}

void ALevelPreparedActor::PreInitializeComponents()
{
	Super::PreInitializeComponents();

	OnPreInitializeComponents();
}

// Called every frame
void ALevelPreparedActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelPreparedActor::InitLevelEffects()
{
	//Landscape Material RVTV Array Texture
	m_pCachedMtlRVTVArrayTex = CreateMaterialRVTVArrayTexture(m_iMtlRVTVArrayTexSizeX, m_iMtlRVTVArrayTexSizeY);
	if (m_pCachedMtlRVTVArrayTex && IsValid(m_pLandscape))
		m_pLandscape->SetLandscapeMaterialTextureParameterValue(m_MtlRVTVArrayTexParamName, m_pCachedMtlRVTVArrayTex);
}