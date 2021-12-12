// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/001_RTD/TestOrbityActor.h"

// Sets default values
ATestOrbityActor::ATestOrbityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_fCamAngle = -1.0f;
	m_fCamR = 0.0f;
	m_fCamAngleOffset = 0.0f;
	m_fCamAngleSpeed = 0.0f;
}

// Called when the game starts or when spawned
void ATestOrbityActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestOrbityActor::Tick(float fDeltaTime)
{
	Super::Tick(fDeltaTime);

	if (m_fCamAngle >= -0.5f)
	{
		m_fCamAngle += m_fCamAngleSpeed * fDeltaTime;

		float fFixedCamAngle = m_fCamAngle + m_fCamAngleOffset;
		fFixedCamAngle = FMath::Fmod(fFixedCamAngle, 360.0f);
		float fCamRadian = FMath::DegreesToRadians(fFixedCamAngle);
		FVector vLoc = GetOwner()->GetActorLocation();
		vLoc.X += m_fCamR * FMath::Cos(fCamRadian);
		vLoc.Y += m_fCamR * FMath::Sin(fCamRadian);
		vLoc.Z = m_transStand.GetLocation().Z;
		SetActorLocation(vLoc);

		float fFixedYaw = m_fCamAngle + m_transStand.Rotator().Yaw + 180.0f;
		fFixedYaw = FMath::Fmod(fFixedYaw, 360.0f) - 180.0f;
		FRotator rRot(m_transStand.Rotator().Pitch, fFixedYaw, m_transStand.Rotator().Roll);
		SetActorRotation(rRot);

		if (m_fCamAngle >= 360.0f)
		{
			m_fCamAngle = -1.0f;
			m_DeleEnded.ExecuteIfBound();
		}
	}
}

void ATestOrbityActor::Orbit(float fTime, AActor* pBaseActor, const FTransform& transStand, FSimpleDynamicDelegate DeleEnded)
{
	check(pBaseActor);
	m_transStand = transStand;
	m_DeleEnded = DeleEnded;

	FVector&& vOrbitArm = GetActorLocation() - pBaseActor->GetActorLocation();
	vOrbitArm.Z = 0.0f;
	m_fCamR = vOrbitArm.Size();
	m_fCamAngleOffset = vOrbitArm.Rotation().Yaw;
	m_fCamAngleSpeed = 360.0f / fTime;
	m_fCamAngle = 0.0f;
}