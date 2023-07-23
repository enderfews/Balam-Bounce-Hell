// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HeatMapActor.generated.h"

class UInstancedStaticMeshComponent;
class USceneComponent;
class UTxtFileAsset;

UCLASS()
class AUTOMATINATOR_API AHeatMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AHeatMapActor();
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "Scene Root")
	USceneComponent* mc_SceneRoot;
	UPROPERTY(VisibleAnywhere,Category="Components",DisplayName="FPS High Square")
	UInstancedStaticMeshComponent* mc_FPSHighSquare;
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "FPS Mid Square")
	UInstancedStaticMeshComponent* mc_FPSMidSquare;
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "FPS Low Square")
	UInstancedStaticMeshComponent* mc_FPSLowSquare;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Upper")
	int m_iFPSUpper;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Lower")
	int m_iFPSLower;


	UPROPERTY(EditAnywhere, Category="Attributes", DisplayName="Array Lines")
	TArray<FString> m_TLines;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Attributes", DisplayName = "File Asset")
	UTxtFileAsset* m_pFileAsset;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
};
