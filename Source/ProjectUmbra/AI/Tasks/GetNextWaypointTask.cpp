// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/GetNextWaypointTask.h"
#include "GetNextWaypointTask.h"
#include "Engine/Engine.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Items/Waypoint.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"

UGetNextWaypointTask::UGetNextWaypointTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}
EBTNodeResult::Type UGetNextWaypointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	AAIController* pController = Cast<AAIController>(OwnerComp.GetOwner());
	ABaseEnemy* pEnemy = pController ? Cast<ABaseEnemy>(pController->GetPawn()) : nullptr;
	if (pEnemy)
	{
		if (pEnemy->m_TWaypoints.Num() > 1)
		{
			int iIndex=pBlackboard->GetValueAsInt(FName("WaypointIndex"));
			pBlackboard->SetValueAsVector(m_FPatrolLocKey.SelectedKeyName, pEnemy->m_TWaypoints[iIndex]->GetActorLocation());
			pBlackboard->SetValueAsInt(FName("WaypointIndex"), (iIndex + 1) % pEnemy->m_TWaypoints.Num());
			return EBTNodeResult::Succeeded;

		}
		else {
			#ifdef WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in get next waypoint task: YOU MUST PROVIDE AT LEAST 2 WAYPOINTS"));
			#endif 
			UE_LOG(LogTemp, Error, TEXT("Error in get next waypoint task: YOU MUST PROVIDE AT LEAST 2 WAYPOINTS"));
			return EBTNodeResult::Failed;
		}
	}
	else {
		#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in get next waypoint task: Could not get the enemy reference"));
		#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in get next waypoint task: Could not get the enemy reference"));
		return EBTNodeResult::Failed;
	}
}

FString UGetNextWaypointTask::GetStaticDescription() const
{
	return FString("Get the next patrol location of the enemy");
}
