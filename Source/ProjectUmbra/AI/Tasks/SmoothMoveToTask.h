// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Components/SplineComponent.h"
#include "SmoothMoveToTask.generated.h"

class ANormalEnemy;
class ASplineMovementCurve;
UCLASS()
class PROJECTUMBRA_API USmoothMoveToTask : public UBTTaskNode
{
	GENERATED_BODY()
private:
	UBehaviorTreeComponent* m_pBT;
public:
	USmoothMoveToTask();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Attack mode")
		bool m_bAttackMode;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Location")
		FBlackboardKeySelector m_FLocation;
	ASplineMovementCurve* m_pCurrentCurve;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UFUNCTION()
		void CurveFinished(ASplineMovementCurve* pSpline);
private:
	//FSplineCurves m_FSpline;

	//void GetLocationAtDistanceAlongSpline(float _fDistance, ESplineCoordinateSpace::Type _eCoordinateSpace, ANormalEnemy* _pEnemy,FVector& vLoc_) const;
	//void AddLocationOnSpline();
};
