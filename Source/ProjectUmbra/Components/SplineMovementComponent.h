// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SplineMovementComponent.generated.h"

class ASplinePath;
UENUM()
enum class ESplineMovementType : uint8
{
	Once,
	PingPong,
	Loop
};

class USplineComponent;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTUMBRA_API USplineMovementComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	float m_fCurrentStopTimeAtStart = 0.0f;
	float m_fCurrentStopTimeAtEnd = 0.0f;
	float m_fMovementDirection = 1.0f;
	float m_fCurrentDistance = 0.0f;

	FTimerHandle m_FStopTimerHandle;

	void EnableTick();
	void DisabledTick();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& _FPropertyChangedEvent) override;
#endif

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Path To Follow")
	ASplinePath* m_APathToFollow;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Movement Type")
	ESplineMovementType m_EMovementType = ESplineMovementType::Once;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Speed", meta=(ClampMin=1.0f))
	float m_fSpeed = 500.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Stop Time At End", meta=(ClampMin=0.0f))
	float m_fStopTimeAtEnd = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Stop Time At Start", meta=(ClampMin=0.0f))
	float m_fStopTimeAtStart = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Starting Progress", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fStartingProgress = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Start Enabled", meta=(ClampMin=0.0f, ClampMax=1.0f))
	bool m_bStartEnabled = true;

	// Sets default values for this component's properties
	USplineMovementComponent();
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Start();
	UFUNCTION(BlueprintCallable)
	void Stop();
	UFUNCTION(BlueprintCallable)
	void ReverseMovementDirection();
};
