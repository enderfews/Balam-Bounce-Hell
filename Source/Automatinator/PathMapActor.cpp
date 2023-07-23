// Fill out your copyright notice in the Description page of Project Settings.


#include "PathMapActor.h"
#include "Automatinator/TxtFileAsset.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Components/SceneComponent.h"
#include "Misc/CString.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
APathMapActor::APathMapActor():m_iUpper(60),m_iLower(30),m_TLines(TArray<FString>()),m_pFileAsset(nullptr)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(mc_SceneRoot);
	mc_HighInfluenceSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("High Influence Square"));
	mc_HighInfluenceSquare->SetupAttachment(mc_SceneRoot);
	mc_HighInfluenceSquare->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mc_MidInfluenceSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Mid Influence Square"));
	mc_MidInfluenceSquare->SetupAttachment(mc_SceneRoot);
	mc_MidInfluenceSquare->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	mc_LowInfluenceSquare = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Low Influence Square"));
	mc_LowInfluenceSquare->SetupAttachment(mc_SceneRoot);
	mc_LowInfluenceSquare->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APathMapActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APathMapActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
void APathMapActor::ClearMap()
{

	if (mc_HighInfluenceSquare->GetInstanceCount() != 0) mc_HighInfluenceSquare->ClearInstances();
	if (mc_MidInfluenceSquare->GetInstanceCount() != 0) mc_MidInfluenceSquare->ClearInstances();
	if (mc_LowInfluenceSquare->GetInstanceCount() != 0) mc_LowInfluenceSquare->ClearInstances();
	m_TMapCount.Empty();
}
void APathMapActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (mc_HighInfluenceSquare->GetInstanceCount() != 0) mc_HighInfluenceSquare->ClearInstances();
	if (mc_MidInfluenceSquare->GetInstanceCount() != 0) mc_MidInfluenceSquare->ClearInstances();
	if (mc_LowInfluenceSquare->GetInstanceCount() != 0) mc_LowInfluenceSquare->ClearInstances();
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
				mc_HighInfluenceSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), It.Key(), FVector(1.f, 1.f, 1.f)));
			}
			else if(It.Value() < m_iUpper && It.Value()>=m_iLower)
			{
				mc_MidInfluenceSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), It.Key(), FVector(1.f, 1.f, 1.f)));
			}
			else
			{
				mc_LowInfluenceSquare->AddInstanceWorldSpace(FTransform(FRotator(0.f, 0.f, 0.f), It.Key(), FVector(1.f, 1.f, 1.f)));
			}
		}
	}
}

