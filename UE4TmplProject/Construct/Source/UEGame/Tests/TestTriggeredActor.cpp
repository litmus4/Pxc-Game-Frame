// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/TestTriggeredActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "TestRunningComponent.h"

// Sets default values
ATestTriggeredActor::ATestTriggeredActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	m_pMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	m_pMeshComp->SetSimulatePhysics(true);
	m_pMeshComp->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	m_pMeshComp->SetCollisionResponseToAllChannels(ECR_Block);
	RootComponent = m_pMeshComp;

	m_pCppBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("CppBoxComp"));
	m_pCppBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_pCppBoxComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	m_pCppBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATestTriggeredActor::OnBoxOverlapped);
	m_pCppBoxComp->SetupAttachment(RootComponent);

	m_pBpBoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BpBoxComp"));
	m_pBpBoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	m_pBpBoxComp->SetCollisionResponseToAllChannels(ECR_Overlap);
	m_pBpBoxComp->OnComponentBeginOverlap.AddDynamic(this, &ATestTriggeredActor::OnBoxOverlapped);
	m_pBpBoxComp->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void ATestTriggeredActor::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATestTriggeredActor::OnBoxOverlapped(UPrimitiveComponent* pOverlappedComp, AActor* pOtherActor, UPrimitiveComponent* pOtherComp,
	int32 iOtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValid(pOtherActor) || !pOtherActor->IsA<APawn>())
		return;

	UTestRunningComponent* pTestComp = FindComponentByClass<UTestRunningComponent>();
	if (!IsValid(pTestComp) || pTestComp->IsRunning()) return;

	check(pOverlappedComp);
	FString&& sCompName = pOverlappedComp->GetName();
	if (sCompName == TEXT("CppBoxComp"))
	{
		if (pTestComp->IsWithParam())
		{
			FSharedSignature ParamSig;
			pTestComp->MakeParameterByOverlappingActor(pOtherActor, ParamSig);
			pTestComp->RunCppTestWithParam(ParamSig);
		}
		else
			pTestComp->RunCppTestNoParam();
	}
	else if (sCompName == TEXT("BpBoxComp"))
	{
		if (pTestComp->IsWithParam())
		{
			FSharedSignature ParamSig;
			pTestComp->MakeParameterByOverlappingActor(pOtherActor, ParamSig);
			pTestComp->RunBPTestWithParam(ParamSig);
		}
		else
			pTestComp->RunBPTestNoParam();
	}
}

// Called every frame
void ATestTriggeredActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

