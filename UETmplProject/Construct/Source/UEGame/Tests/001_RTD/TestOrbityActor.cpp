// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/001_RTD/TestOrbityActor.h"

// Sets default values
ATestOrbityActor::ATestOrbityActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_dCamAngle = -1.0;
	m_dCamR = 0.0;
	m_dCamAngleOffset = 0.0;
	m_dCamAngleSpeed = 0.0;
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

	if (m_dCamAngle >= -0.5)
	{
		m_dCamAngle += m_dCamAngleSpeed * fDeltaTime;

		double dFixedCamAngle = m_dCamAngle + m_dCamAngleOffset;
		dFixedCamAngle = FMath::Fmod(dFixedCamAngle, 360.0);
		double dCamRadian = FMath::DegreesToRadians(dFixedCamAngle);
		FVector vLoc = GetOwner()->GetActorLocation();
		vLoc.X += m_dCamR * FMath::Cos(dCamRadian);
		vLoc.Y += m_dCamR * FMath::Sin(dCamRadian);
		vLoc.Z = m_transStand.GetLocation().Z;
		SetActorLocation(vLoc);

		double dFixedYaw = m_dCamAngle + m_transStand.Rotator().Yaw + 180.0;
		dFixedYaw = FMath::Fmod(dFixedYaw, 360.0) - 180.0;
		FRotator rRot(m_transStand.Rotator().Pitch, dFixedYaw, m_transStand.Rotator().Roll);
		SetActorRotation(rRot);

		if (m_dCamAngle >= 360.0)
		{
			m_dCamAngle = -1.0;
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
	vOrbitArm.Z = 0.0;
	m_dCamR = vOrbitArm.Size();
	m_dCamAngleOffset = vOrbitArm.Rotation().Yaw;
	m_dCamAngleSpeed = 360.0 / fTime;
	m_dCamAngle = 0.0;
}