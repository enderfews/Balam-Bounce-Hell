// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "SplineMovementCurve.generated.h"

class USplineComponent;
class ANormalEnemy;
class UCurveFloat;
UCLASS()
class PROJECTUMBRA_API ASplineMovementCurve : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASplineMovementCurve();

	ANormalEnemy* m_pEnemyRef;
	bool m_bAttackMode;
	FTimeline m_TLMoveAlongSpline;
	UCurveFloat* m_pCurve;
	UFUNCTION()
		void MoveAlongSplineFunc(float alpha);
	UFUNCTION()
		void OnPathEnded();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Curve movement")
		USplineComponent* mc_UCurve;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSplineEnded,ASplineMovementCurve*, pCurveRef);
	UPROPERTY(BlueprintAssignable)
		FOnSplineEnded m_OnSplineEnded;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void PerformSmoothMove(ANormalEnemy* pEnemy,float fEnemySpeed);
	void AddPointsToSpline(TArray<FVector>& _TPoints);
};
