// Fill out your copyright notice in the Description page of Project Settings.


#include "Automatinator/HeatMapActor.h"
#include "Automatinator/TxtFileAsset.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Misc/CString.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
AHeatMapActor::AHeatMapActor(): m_iFPSUpper(60), m_iFPSLower(30)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(mc_SceneRoot);
	mc_FPSHighSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FPS High Square"));
	mc_FPSHighSquare->SetupAttachment(mc_SceneRoot);
	mc_FPSMidSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FPS Mid Square"));
	mc_FPSMidSquare->SetupAttachment(mc_SceneRoot);
	mc_FPSLowSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("FPS Low Square"));
	mc_FPSLowSquare->SetupAttachment(mc_SceneRoot);

}

// Called when the game starts or when spawned
void AHeatMapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHeatMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AHeatMapActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (mc_FPSHighSquare->GetInstanceCount() != 0) mc_FPSHighSquare->ClearInstances();
	if (mc_FPSMidSquare->GetInstanceCount() != 0) mc_FPSMidSquare->ClearInstances();
	if (mc_FPSLowSquare->GetInstanceCount() != 0) mc_FPSLowSquare->ClearInstances();
	FString left, right;
	if (m_pFileAsset!=nullptr) {
		for (int i = 0; i < m_pFileAsset->m_TLines.Num(); i++) {
			UKismetStringLibrary::Split(m_pFileAsset->m_TLines[i], FString(TEXT("||")), left, right);
			FVector vPosition;
			vPosition.InitFromString(left);
			vPosition += FVector(50.f, 50.f, 50.f);
			int iFPS = FCString::Atoi(*right);
			if (iFPS >= m_iFPSUpper)
			{
				mc_FPSHighSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), vPosition, FVector(1.f, 1.f, 1.f)));
			}
			else if (iFPS < m_iFPSUpper && iFPS >= m_iFPSLower)
			{
				mc_FPSMidSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), vPosition, FVector(1.f, 1.f, 1.f)));
			}
			else
			{
				mc_FPSLowSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), vPosition, FVector(1.f, 1.f, 1.f)));
			}
		}
	}
}

