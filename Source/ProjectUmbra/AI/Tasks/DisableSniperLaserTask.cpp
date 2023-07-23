// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/DisableSniperLaserTask.h"
#include "DisableSniperLaserTask.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"

UDisableSniperLaserTask::UDisableSniperLaserTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UDisableSniperLaserTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ASniperEnemyController* pEnemyController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pEnemyController ? pEnemyController->m_pSniperRef : nullptr;
	if (pEnemy)
	{
		pEnemy->AbortLaser();
		return EBTNodeResult::Succeeded;
	}
	else {
		return EBTNodeResult::Failed;
	}
}

EBTNodeResult::Type UDisableSniperLaserTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ASniperEnemyController* pEnemyController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pEnemyController ? pEnemyController->m_pSniperRef : nullptr;
	if (pEnemy)
	{
		pEnemy->AbortLaser();
		return EBTNodeResult::Aborted;
	}
	else {
		return EBTNodeResult::Aborted;
	}
}

FString UDisableSniperLaserTask::GetStaticDescription() const
{
	return FString();
}
