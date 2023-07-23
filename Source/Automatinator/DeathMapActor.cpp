// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathMapActor.h"
#include "Automatinator/TxtFileAsset.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Misc/CString.h"
#include "Kismet/KismetStringLibrary.h"
// Sets default values
ADeathMapActor::ADeathMapActor() :m_iUpper(60), m_iLower(30), m_TLines(TArray<FString>()), m_pFileAsset(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(mc_SceneRoot);
	mc_HighDeathSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("High Death Square"));
	mc_HighDeathSquare->SetupAttachment(mc_SceneRoot);
	mc_HighDeathSquare->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mc_MidDeathSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Mid Death Square"));
	mc_MidDeathSquare->SetupAttachment(mc_SceneRoot);
	mc_MidDeathSquare->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mc_LowDeathSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Low Death Square"));
	mc_LowDeathSquare->SetupAttachment(mc_SceneRoot);
	mc_LowDeathSquare->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void ADeathMapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ADeathMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADeathMapActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (mc_HighDeathSquare->GetInstanceCount() != 0) mc_HighDeathSquare->ClearInstances();
	if (mc_MidDeathSquare->GetInstanceCount() != 0) mc_MidDeathSquare->ClearInstances();
	if (mc_LowDeathSquare->GetInstanceCount() != 0) mc_LowDeathSquare->ClearInstances();
	FString left, right;
	if (m_pFileAsset != nullptr)
	{
		for (int i = 0; i < m_pFileAsset->m_TLines.Num(); i++)
		{
			UKismetStringLibrary::Split(m_pFileAsset->m_TLines[i], FString(TEXT("||")), left, right);
			FVector vPosition;
			vPosition.InitFromString(left);
			vPosition += FVector(50.f, 50.f, 50.f);
			int iCount = FCString::Atoi(*right);
			if (m_TMapCount.Contains(vPosition))
			{
				m_TMapCount[vPosition] += iCount;
			}
			else {
				m_TMapCount.Add(vPosition, iCount);
			}
		}
		for (auto It = m_TMapCount.CreateConstIterator(); It; ++It)
		{
			if (It.Value() >= m_iUpper)
			{
				mc_HighDeathSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), It.Key(), FVector(3.f, 3.f, 1.f)));
			}
			else if (It.Value() < m_iUpper && It.Value() >= m_iLower)
			{
				mc_MidDeathSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), It.Key(), FVector(3.f, 3.f, 1.f)));
			}
			else
			{
				mc_LowDeathSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), It.Key(), FVector(3.f, 3.f, 1.f)));
			}
		}
	}
}

void ADeathMapActor::ClearMap()
{
	if (mc_HighDeathSquare->GetInstanceCount() != 0) mc_HighDeathSquare->ClearInstances();
	if (mc_MidDeathSquare->GetInstanceCount() != 0) mc_MidDeathSquare->ClearInstances();
	if (mc_LowDeathSquare->GetInstanceCount() != 0) mc_LowDeathSquare->ClearInstances();
	m_TMapCount.Empty();
}

