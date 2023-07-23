// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/GetNextTeleportWaypointTask.h"
#include "ProjectUmbra/Items/Waypoint.h"
#include "Engine/Engine.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"


UGetNextTeleportWaypointTask::UGetNextTeleportWaypointTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}
EBTNodeResult::Type UGetNextTeleportWaypointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ASniperEnemyController* pController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pController ? pController->m_pSniperRef : nullptr;
	if (pEnemy)
	{
		if (pEnemy->m_TWaypoints.Num() > 1)
		{
			int iIndex = pBlackboard->GetValueAsInt(FName("WaypointIndex"));
			if (pEnemy->m_eTeleportPattern == ESniperTeleportPattern::FollowWaypointsLinearly)
			{
				if (!pEnemy->IsTeleportEffectPlaying())
				{
					pBlackboard->SetValueAsVector(m_FPatrolLocKey.SelectedKeyName, pEnemy->m_TWaypoints[iIndex]->GetActorLocation());
					pBlackboard->SetValueAsInt(FName("WaypointIndex"), (iIndex + 1) % pEnemy->m_TWaypoints.Num());
				}
				return EBTNodeResult::Succeeded;
			}
			else 
			{
				if (!pEnemy->IsTeleportEffectPlaying())
				{
					TArray<int> TIndexes;
					for (int i = 0; i < pEnemy->m_TWaypoints.Num(); ++i)
					{
						TIndexes.Add(i);
					}
					float fDistance = 999999999.f;
					float fCurrentDistance;
					int iSearch = 0;
					for (int i=0; i<pEnemy->m_TWaypoints.Num();++i)
					{
						fCurrentDistance = (pEnemy->m_TWaypoints[i]->GetActorLocation() - pEnemy->GetActorLocation()).Size();
						if (fCurrentDistance < fDistance)
						{
							fDistance = fCurrentDistance;
							iSearch = i;
						}
					}
					TIndexes.Remove(iSearch);
					int iNewIndex = UKismetMathLibrary::RandomIntegerInRange(0, TIndexes.Num() - 1);
					pBlackboard->SetValueAsVector(m_FPatrolLocKey.SelectedKeyName, pEnemy->m_TWaypoints[TIndexes[iNewIndex]]->GetActorLocation());
					pBlackboard->SetValueAsInt(FName("WaypointIndex"), TIndexes[iNewIndex]);
				}
				return EBTNodeResult::Succeeded;
			}

		}
		else {
			#ifdef WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in get next teleport waypoint task: YOU MUST PROVIDE AT LEAST 2 WAYPOINTS"));
			#endif 
			UE_LOG(LogTemp, Error, TEXT("Error in get next teleport waypoint task: YOU MUST PROVIDE AT LEAST 2 WAYPOINTS"));
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
FString UGetNextTeleportWaypointTask::GetStaticDescription() const
{
	return FString("Get Next teleport waypoint");
}
