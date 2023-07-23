// Fill out your copyright notice in the Description page of Project Settings.


#include "StuckMapActor.h"
#include "Automatinator/TxtFileAsset.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Misc/CString.h"
#include "Kismet/KismetStringLibrary.h"
// Sets default values
AStuckMapActor::AStuckMapActor():m_iStuckLimit(10)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(mc_SceneRoot);
	mc_StuckSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Stuck Squares"));
	mc_StuckSquare->SetupAttachment(mc_SceneRoot);
}

// Called when the game starts or when spawned
void AStuckMapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStuckMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStuckMapActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (mc_StuckSquare->GetInstanceCount() != 0) mc_StuckSquare->ClearInstances();

	FString left, right;
	if (m_pFileAsset != nullptr)
	{
		for (int i = 0; i < m_pFileAsset->m_TLines.Num(); i++) 
		{
			UKismetStringLibrary::Split(m_pFileAsset->m_TLines[i], FString(TEXT("||")), left, right);
			FVector vPosition;
			vPosition.InitFromString(left);
			vPosition += FVector(50.f, 50.f, 50.f);
			int iStuckCount = FCString::Atoi(*right);
			if (iStuckCount >= m_iStuckLimit)
			{
				mc_StuckSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), vPosition, FVector(3.f, 3.f, 1.f)));
			}
		}
	}

}

