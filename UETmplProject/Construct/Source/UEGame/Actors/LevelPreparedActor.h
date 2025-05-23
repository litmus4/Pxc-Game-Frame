// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ActorStructs.h"
#include "LevelPreparedActor.generated.h"

class UTexture2D;
class UMaterialInstance;
class ALandscapeProxy;
class ARuntimeVirtualTextureVolume;

UCLASS()
class UEGAME_API ALevelPreparedActor : public AActor
{
	GENERATED_BODY()

public:
	struct SRVTVExtension
	{
		ARuntimeVirtualTextureVolume* pVolume;
		uint64 ulRegionCoord;
	};
	
public:	
	// Sets default values for this actor's properties
	ALevelPreparedActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintNativeEvent)
	void OnPreInitializeComponents();
	virtual void OnPreInitializeComponents_Implementation() {}

	UFUNCTION(BlueprintPure)
	UTexture2D* CreateMaterialRVTVArrayTexture(int32 iSizeX, int32 iSizeY);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<FName, FSubLevelLinkInfo> m_tmapSubLevelLinkInfos;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	ALandscapeProxy* m_pLandscape;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	FVector2D m_v2RVTVCornerZero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	double m_dRVTVSideLength;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	int32 m_iMtlRVTVArrayTexSizeX;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	int32 m_iMtlRVTVArrayTexSizeY;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	FName m_MtlRVTVArrayTexParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	FName m_MtlRVTVCornerZeroParamName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = RVTV)
	FName m_MtlRVTVSideLengthParamName;

	UPROPERTY(BlueprintReadOnly)
	UTexture2D* m_pCachedMtlRVTVArrayTex;

public:
	virtual void PreRegisterAllComponents() override;
	virtual void PreInitializeComponents() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	void InitLevelEffects();
};
