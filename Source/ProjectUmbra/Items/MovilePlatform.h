// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "MovilePlatform.generated.h"

class USoundManager;
class UNiagaraSystem;
class UFMODAudioComponent;
class UFMODEvent;
class UStaticMeshComponent;
class UBoxComponent;
class USplineComponent;
class UCurveFloat;
class USceneComponent;
class UEnemyDeathDetectorComponent;
UCLASS()
class PROJECTUMBRA_API AMovilePlatform : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMovilePlatform();
	//Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Root")
		USceneComponent* mc_URoot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "PlatformMesh")
		UStaticMeshComponent* mc_UPlatformMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Box")
		UBoxComponent* mc_UBox;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Spline")
		USplineComponent* mc_USpline;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Death Detector")
		UEnemyDeathDetectorComponent* mc_UDeathDetector;
	//Attributes
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Use Waypoints")
		bool m_bUseWaypoints;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Waypoints")
		TArray<AActor*> m_TWaypoints;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Platform speeds")
		TArray<float> m_TPlatformSpeeds;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Pauses between points")
		TArray<float> m_TPausesBetweenPoints;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Is Circular")
		bool m_bIsCircular;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Reach distance")
		float m_fReachDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Time line length")
		float m_fTimeLineLength;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Needs the player to be activated")
		bool m_bNeedsPlayer;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform Config", DisplayName = "Activates killing enemies")
		bool m_bActivatesKillingEnemies;
		bool m_bPlatformUnlocked;
		bool m_bTransitionBetweenPlatforms;
		float m_fTimeToGetVel;

	//Sounds
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Idle FMOD Event"))
	UFMODEvent* m_UIdleFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Stop FMOD Event"))
	UFMODEvent* m_UStopFMODEvent;
	
private:
	bool m_bSameArraySizes;
	bool m_bProvidedAllData;
	bool m_bPlayerDetected;
	int m_iIndex;
	FVector m_FDstLoc;
	FVector m_vLastLoc;
	FTimerHandle m_FPlatformTimer;
	bool m_bStartAgainSpline;

	FTimerHandle m_DelayTimer;
	UCurveFloat* m_UCurve;
	FTimeline m_FTimeline;

	UPROPERTY()
	USoundManager* m_USndManager;
	UPROPERTY()
	UFMODAudioComponent* m_UIdleAudioComponent;
	
public:
	UFUNCTION()
		void SplineTLFunc(float alpha);
	UFUNCTION()
		void OnTimelineFinished();
	UFUNCTION()
		void DelaySplineFalse();
	UFUNCTION()
		void DelaySplineTrue();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform);

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
		void StopPlatform();
	UFUNCTION(BlueprintCallable)
		void ResumePlatform();
	void MovePlatform();
	void ReturnPlatform();

	void Delay1();
	void Delay2();
	void Delay3();
	void Delay4();


	UFUNCTION()
		void OnAllEnemiesDeathPlatform();
};
