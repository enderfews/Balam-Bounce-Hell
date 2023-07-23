// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/SmoothMoveToTask.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "Components/SplineComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "ProjectUmbra/AI/SmartObjects/SplineMovementCurve.h"

//TArray<ASplineMovementCurve*> USmoothMoveToTask::TCurvePool = TArray<ASplineMovementCurve*>();

USmoothMoveToTask::USmoothMoveToTask():m_bAttackMode(false)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}
EBTNodeResult::Type USmoothMoveToTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);
	if (m_pCurrentCurve)
	{
		m_pCurrentCurve->m_OnSplineEnded.RemoveDynamic(this, &USmoothMoveToTask::CurveFinished);
		m_pCurrentCurve->m_TLMoveAlongSpline.Stop();
		m_pCurrentCurve->mc_UCurve->ClearSplinePoints(true);
		AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
		if (pGM)
		{
			pGM->TCurvePool.Add(m_pCurrentCurve);
		}
		else {
			m_pCurrentCurve->Destroy();
		}
		m_pCurrentCurve = nullptr;

		return EBTNodeResult::Aborted;
	}
	else {
		return EBTNodeResult::Failed;
	}
}

EBTNodeResult::Type USmoothMoveToTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	m_pBT = &OwnerComp;
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ANormalEnemyController* pController = pBlackboard? Cast<ANormalEnemyController>(OwnerComp.GetOwner()):nullptr;
	ANormalEnemy* pEnemy = pController ? pController->m_pEnemyRef : nullptr;
	FVector vMeta;
	AActor* pActor=nullptr;
	UNavigationPath* pPath;
	TArray<FVector> TPathPoints;
	if (pEnemy)
	{
		pActor=Cast<AActor>(pBlackboard->GetValueAsObject(m_FLocation.SelectedKeyName));
		if (pActor)
		{
			pPath = UNavigationSystemV1::FindPathToActorSynchronously(GetWorld(), pEnemy->GetActorLocation(), pActor);
			TPathPoints = pPath->PathPoints;
		}
		else
		{
			vMeta = pBlackboard->GetValueAsVector(m_FLocation.SelectedKeyName);
			pPath=UNavigationSystemV1::FindPathToLocationSynchronously(GetWorld(), pEnemy->GetActorLocation(), vMeta);
			TPathPoints = pPath->PathPoints;
		}

		//TODO: Implement movement with spline to perform smooth movement for the AIs
		m_pCurrentCurve =nullptr;
		AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
		if (pGM && pGM->TCurvePool.Num() == 0)
		{
			m_pCurrentCurve = GetWorld()->SpawnActor<ASplineMovementCurve>(ASplineMovementCurve::StaticClass(), pEnemy->GetActorLocation(), pEnemy->GetActorRotation());
			if (m_pCurrentCurve)
			{
				m_pCurrentCurve->AddPointsToSpline(TPathPoints);
				m_pCurrentCurve->m_OnSplineEnded.AddDynamic(this, &USmoothMoveToTask::CurveFinished);
				m_pCurrentCurve->m_bAttackMode = m_bAttackMode;
				m_pCurrentCurve->PerformSmoothMove(pEnemy, pEnemy->GetCharacterMovement()->MaxWalkSpeed);
			}
			else {
				return EBTNodeResult::Failed;
			}
		}
		else if(pGM && pGM->TCurvePool.Num()!=0)
		{
			m_pCurrentCurve = pGM->TCurvePool[0];
			if (m_pCurrentCurve)
			{
				pGM->TCurvePool.Remove(m_pCurrentCurve);
				m_pCurrentCurve->SetActorLocation(pEnemy->GetActorLocation());
				m_pCurrentCurve->SetActorRotation(pEnemy->GetActorRotation());
				m_pCurrentCurve->AddPointsToSpline(TPathPoints);
				m_pCurrentCurve->m_OnSplineEnded.AddDynamic(this, &USmoothMoveToTask::CurveFinished);
				m_pCurrentCurve->PerformSmoothMove(pEnemy, pEnemy->GetCharacterMovement()->MaxWalkSpeed);
			}
			else {
				return EBTNodeResult::Failed;
			}
			
		}

		return EBTNodeResult::InProgress;
	}
	else {
		return EBTNodeResult::Failed;
	}
}

FString USmoothMoveToTask::GetStaticDescription() const
{
	return FString("Performs spline based movement");
}

void USmoothMoveToTask::CurveFinished(ASplineMovementCurve* pSpline)
{
	m_pCurrentCurve->m_OnSplineEnded.RemoveDynamic(this, &USmoothMoveToTask::CurveFinished);
	AUmbraGameMode* pGM = Cast<AUmbraGameMode>(GetWorld()->GetAuthGameMode());
	if (pGM)
	{
		pGM->TCurvePool.Add(pSpline);
	}
	else {
		m_pCurrentCurve->Destroy();
	}
	m_pCurrentCurve = nullptr;
	FinishLatentTask(*m_pBT, EBTNodeResult::Succeeded);
}

//void USmoothMoveToTask::GetLocationAtDistanceAlongSpline(float _fDistance, ESplineCoordinateSpace::Type _eCoordinateSpace, ANormalEnemy* _pEnemy,FVector& vLoc_) const
//{
//	const float fParam = m_FSpline.ReparamTable.Eval(_fDistance, 0.0f);
//	vLoc_ = m_FSpline.Position.Eval(fParam, FVector::ZeroVector);
//	if (_eCoordinateSpace == ESplineCoordinateSpace::World)
//	{
//		vLoc_ = _pEnemy->GetActorTransform().TransformPosition(vLoc_);
//	}
//}
