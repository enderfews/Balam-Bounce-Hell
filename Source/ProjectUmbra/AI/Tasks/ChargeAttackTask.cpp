// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/ChargeAttackTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"

UChargeAttackTask::UChargeAttackTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UChargeAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	m_pBTC = &OwnerComp;
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ANormalEnemyController* pController = pBlackboard ? Cast<ANormalEnemyController>(OwnerComp.GetOwner()): nullptr;
	ANormalEnemy* pEnemy = pController ? pController->m_pEnemyRef : nullptr;
	if (pEnemy)
	{
		pEnemy->m_FOnChargeEnded.AddDynamic(this, &UChargeAttackTask::ChargeEnded);
		pEnemy->ChargeAttack(Cast<ACharacter>(pBlackboard->GetValueAsObject(m_FEntityToCharge.SelectedKeyName)));
		return EBTNodeResult::InProgress;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in charge attack task cpp: Could not get the enemy ref"));
		return EBTNodeResult::Failed;
	}
}

FString UChargeAttackTask::GetStaticDescription() const
{
	return FString("Charge through the entity");
}

void UChargeAttackTask::ChargeEnded()
{
	UBehaviorTreeComponent* OwnerComp = m_pBTC;
	UBlackboardComponent* pBlackboard = OwnerComp->GetBlackboardComponent();
	if (pBlackboard)
	{
		pBlackboard->SetValueAsBool(FName("ChargeAttackUsed"), true);
		FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Error in charge attack task cpp, charge ended function: could not get blackboard ref"));
		FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
	}
}
