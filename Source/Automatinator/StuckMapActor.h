// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "StuckMapActor.generated.h"

class USceneComponent;
class UTxtFileAsset;
class UInstancedStaticMeshComponent;
UCLASS()
class AUTOMATINATOR_API AStuckMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStuckMapActor();
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "Scene Root")
		USceneComponent* mc_SceneRoot;
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "Stuck Square")
		UInstancedStaticMeshComponent* mc_StuckSquare;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Stuck limit")
		int m_iStuckLimit;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "File Asset")
		UTxtFileAsset* m_pFileAsset;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

};
