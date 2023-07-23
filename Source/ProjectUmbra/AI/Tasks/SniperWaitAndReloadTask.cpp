// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/SniperWaitAndReloadTask.h"
#include "Engine/World.h"
#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "SniperWaitAndReloadTask.h"

USniperWaitAndReloadTask::USniperWaitAndReloadTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type USniperWaitAndReloadTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ASniperEnemyController* pController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pController->m_pSniperRef;
	m_pBTC = &OwnerComp;
	if (pBlackboard)
	{
		if (pController)
		{
			if (pEnemy)
			{
				GetWorld()->GetTimerManager().SetTimer(m_FTimer, this, &USniperWaitAndReloadTask::OnDelayFinished, pBlackboard->GetValueAsFloat(m_FSniperCooldownKey.SelectedKeyName), false);
				return EBTNodeResult::InProgress;
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Error in Sniper wait and reload task cpp: Could not get the enemy ref"));
				return EBTNodeResult::Failed;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Sniper wait and reload task cpp: Could not get the controller ref"));
			return EBTNodeResult::Failed;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Sniper wait and reload task cpp: Could not get the blackboard"));
		return EBTNodeResult::Failed;
	}

}

FString USniperWaitAndReloadTask::GetStaticDescription() const
{
	return FString("The sniper waits and then reloads");
}
void USniperWaitAndReloadTask::OnDelayFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(m_FTimer);
	UBehaviorTreeComponent* OwnerComp = m_pBTC;
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}