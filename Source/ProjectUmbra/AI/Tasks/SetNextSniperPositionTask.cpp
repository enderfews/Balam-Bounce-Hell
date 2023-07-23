// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/SetNextSniperPositionTask.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "Engine/World.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"

EBTNodeResult::Type USetNextSniperPositionTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ASniperEnemyController* pEnemyController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	if(pBlackboard)
	{
		if (pEnemyController)
		{
			ASniperEnemy* pEnemy = pEnemyController->m_pSniperRef;
			if (pEnemy)
			{
				pEnemy->PlayTeleportEffect(pBlackboard->GetValueAsVector(m_FNextPositionKey.SelectedKeyName));
				return EBTNodeResult::Succeeded;
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Error in Set sniper position task cpp: Could not get the sniper enemy ref in the controller"));
				return EBTNodeResult::Failed;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Set sniper position task cpp: Could not get the sniper enemy controller ref"));
			return EBTNodeResult::Failed;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Set sniper position task cpp: Could not get the blacboard"));
		return EBTNodeResult::Failed;
	}

}
EBTNodeResult::Type USetNextSniperPositionTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ASniperEnemyController* pEnemyController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());

	if (pBlackboard)
	{
		if (pEnemyController)
		{
			ASniperEnemy* pEnemy = pEnemyController->m_pSniperRef;
			if (pEnemy)
			{
				pEnemy->StopTeleportEffect();
				return EBTNodeResult::Aborted;
			}
			else {
				return EBTNodeResult::Aborted;
			}
		}
		else
		{
			return EBTNodeResult::Aborted;
		}
	}
	else {
		return EBTNodeResult::Aborted;
	}

}
FString USetNextSniperPositionTask::GetStaticDescription() const
{
	return FString("Set the next teleport position of the sniper enemy");
}
