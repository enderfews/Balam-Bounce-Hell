// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/SniperTeleportTask.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"

EBTNodeResult::Type USniperTeleportTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ASniperEnemyController* pController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pController->m_pSniperRef;
	if (pBlackboard)
	{
		if (pController)
		{
			if (pEnemy)
			{
				if (pEnemy->IsTeleportEffectPlaying())
				{
					pEnemy->StopTeleportEffect();
					pEnemy->PlayTeleportEndEffect(pBlackboard->GetValueAsVector(m_FTeleportLocKey.SelectedKeyName));
					pEnemy->SetActorLocation(pBlackboard->GetValueAsVector(m_FTeleportLocKey.SelectedKeyName) + FVector(0.f, 0.f, 100.f));

					//pBlackboard->SetValueAsFloat(m_FTeleportCountKey.SelectedKeyName, pBlackboard->GetValueAsFloat(m_FTeleportCountKey.SelectedKeyName) + 1.f);
				}
				return EBTNodeResult::Aborted;
				
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Error in Sniper teleport task cpp: Could not get the normal enemy ref"));
				return EBTNodeResult::Aborted;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error in Sniper teleport task cpp: Could not get the controller ref"));
			return EBTNodeResult::Aborted;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Sniper teleport task cpp: Could not get the blackboard"));
		return EBTNodeResult::Aborted;
	}
}

EBTNodeResult::Type USniperTeleportTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ASniperEnemyController* pController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pController->m_pSniperRef;
	if (pBlackboard)
	{
		if (pController)
		{
			if (pEnemy)
			{
				if (pEnemy->IsTeleportEffectPlaying())
				{
					pEnemy->StopTeleportEffect();
					pEnemy->PlayTeleportEndEffect(pBlackboard->GetValueAsVector(m_FTeleportLocKey.SelectedKeyName));
					pEnemy->SetActorLocation(pBlackboard->GetValueAsVector(m_FTeleportLocKey.SelectedKeyName) + FVector(0.f, 0.f, 100.f));
					
					pBlackboard->SetValueAsFloat(m_FTeleportCountKey.SelectedKeyName, pBlackboard->GetValueAsFloat(m_FTeleportCountKey.SelectedKeyName) + 1.f);
				}
				return EBTNodeResult::Succeeded;
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Error in Sniper teleport task cpp: Could not get the normal enemy ref"));
				return EBTNodeResult::Failed;
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Error in Sniper teleport task cpp: Could not get the controller ref"));
			return EBTNodeResult::Failed;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Sniper teleport task cpp: Could not get the blackboard"));
		return EBTNodeResult::Failed;
	}
}

FString USniperTeleportTask::GetStaticDescription() const
{
	return FString("Teleports the sniper to the marked area and set the mark invisible");
}
