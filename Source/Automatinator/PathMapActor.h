// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PathMapActor.generated.h"


class UInstancedStaticMeshComponent;
class USceneComponent;
class UTxtFileAsset;

UCLASS()
class AUTOMATINATOR_API APathMapActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APathMapActor();
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "Scene Root")
		USceneComponent* mc_SceneRoot;
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "High Influence Square")
		UInstancedStaticMeshComponent* mc_HighInfluenceSquare;
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "Mid Influence Square")
		UInstancedStaticMeshComponent* mc_MidInfluenceSquare;
	UPROPERTY(VisibleAnywhere, Category = "Components", DisplayName = "Low Influence Square")
		UInstancedStaticMeshComponent* mc_LowInfluenceSquare;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Upper")
		int m_iUpper;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Lower")
		int m_iLower;

	UPROPERTY(EditAnywhere, Category = "Attributes", DisplayName = "Array Lines")
		TArray<FString> m_TLines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "File Asset")
		UTxtFileAsset* m_pFileAsset;
	UPROPERTY(EditAnywhere, Category = "Attributes", DisplayName = "Count Map")
		TMap<FVector, int> m_TMapCount;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;
	void ClearMap();

};
