// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LevelPreparedActor.h"
#include "Kismet/GameplayStatics.h"
#include "VT/RuntimeVirtualTextureVolume.h"
#include "Engine/Texture2D.h"
#include "LandscapeProxy.h"
#include "PublicDefinitions/ObjectNameDef.h"
#include <unordered_map>

// Sets default values
ALevelPreparedActor::ALevelPreparedActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_dRVTVSideLength = 1.0;
	m_iMtlRVTVArrayTexSizeX = 16;
	m_iMtlRVTVArrayTexSizeY = 16;
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
	int32 iPixelNum = iSizeX * iSizeY;
	verify(tarrVolumes.Num() <= iPixelNum);
	verify(FMath::Abs(m_dRVTVSideLength - 0.0) > 0.000001);

	TArray<SRVTVExtension> tarrExts;
	for (AActor* pVolumeActor : tarrVolumes)
	{
		SRVTVExtension Ext;
		Ext.pVolume = Cast<ARuntimeVirtualTextureVolume>(pVolumeActor);
		if (IsValid(Ext.pVolume) && Ext.pVolume->GetActorLabel().Find(TEXT(UOBJECTNAME_VHF)) != INDEX_NONE)
		{
			double dX = Ext.pVolume->GetActorLocation().X - m_v2RVTVCornerZero.X;
			if (FMath::Abs(dX - 0.0) > 0.000001)
				dX /= m_dRVTVSideLength;
			uint32 uX = (uint32)dX;
			int32 iXPrecChk = (int32)(dX * 10) % 10;
			if (iXPrecChk == 9) uX++;

			double dY = Ext.pVolume->GetActorLocation().Y - m_v2RVTVCornerZero.Y;
			if (FMath::Abs(dY - 0.0) > 0.000001)
				dY /= m_dRVTVSideLength;
			uint32 uY = (uint32)dY;
			int32 iYPrecChk = (int32)(dY * 10) % 10;
			if (iYPrecChk == 9) uY++;

			Ext.ulRegionCoord = (((uint64)uY << 32) | uX);
			tarrExts.Add(Ext);
		}
	}
	
	tarrExts.Sort([](const SRVTVExtension& Ext1, const SRVTVExtension& Ext2) {
		return Ext1.ulRegionCoord < Ext2.ulRegionCoord;
	});

	std::unordered_map<uint64, uint32> mapExists;
	for (uint32 u = 0, uNum = (uint32)tarrExts.Num(); u < uNum; ++u)
	{
		SRVTVExtension& Ext = tarrExts[u];
		mapExists.insert(std::make_pair(Ext.ulRegionCoord, u));
	}

	UTexture2D* pResTexture2D = UTexture2D::CreateTransient(iSizeX, iSizeY, PF_R32G32B32A32_UINT);
	uint32* pPixels = new uint32[iSizeX * iSizeY * 4];
	for (int32 i = 0; i < iPixelNum; ++i)
	{
		int32 iY = i / iSizeX, iX = i % iSizeX;
		std::unordered_map<uint64, uint32>::iterator iter = mapExists.find(((uint64)iY << 32) | iX);
		if (iter != mapExists.end())
		{
			pPixels[i * 4] = 1U;
			pPixels[i * 4 + 1] = iter->second;
		}
		else
		{
			pPixels[i * 4] = 0U;
			pPixels[i * 4 + 1] = 0U;
		}
		pPixels[i * 4 + 2] = 0U;
		pPixels[i * 4 + 3] = 0U;
	}
	void* pTextureData = pResTexture2D->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(pTextureData, pPixels, sizeof(uint32) * 4 * iPixelNum);
	pResTexture2D->GetPlatformData()->Mips[0].BulkData.Unlock();
	pResTexture2D->UpdateResource();
	return pResTexture2D;
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