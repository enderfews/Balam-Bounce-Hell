// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "MovingTrain.generated.h"

UENUM()
enum class EMovementType : uint8
{
	Once,
	PingPong,
	Loop
};

UCLASS()
class PROJECTUMBRA_API AMovingTrain : public AActor
{
	GENERATED_BODY()

private:
	//Control variables for movement
	float m_fCurrentStopTimeAtStart = 0.0f;
	float m_fCurrentStopTimeAtEnd = 0.0f;
	float m_fMovementDirection = 1.0f;
	float m_fCurrentDistance = 0.0f;
	float m_bStop = false;

	//Used to activate loop on spline based on type of movement
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	
protected:
	
	virtual void BeginPlay() override;

public:
	//Scene Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Scene Root")
	USceneComponent* mc_USceneRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Train Root")
	USceneComponent* mc_UTrainRoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Train Mesh")
	UStaticMeshComponent* mc_UTrainMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Path")
	USplineComponent* mc_UPath;

	//Configuration
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Movement Type")
	EMovementType m_EMovementType = EMovementType::Once;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Speed", meta=(ClampMin=1.0f))
	float m_fSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Stop Time At End", meta=(ClampMin=0.0f))
	float m_fStopTimeAtEnd = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Stop Time At Start", meta=(ClampMin=0.0f))
	float m_fStopTimeAtStart = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Starting Progress", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fStartingProgress = 0.0f;
	
	AMovingTrain();
	
	virtual void Tick(float DeltaTime) override;

};
