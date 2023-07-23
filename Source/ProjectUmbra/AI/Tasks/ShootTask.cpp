// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootTask.h"

#include "AIController.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"

EBTNodeResult::Type UShootTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AEnemyController = Cast<AAIController>(OwnerComp.GetOwner());

	if(AEnemyController == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ABaseEnemy* AEnemy = Cast<ABaseEnemy>(AEnemyController->GetPawn());
	
	if(AEnemy == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AEnemy->ShootBullet();
	
	return EBTNodeResult::Succeeded;
}

FString UShootTask::GetStaticDescription() const
{
	return TEXT("Triggers the shoot function of an enemy, if the user is not derived from ABaseEnemy the task will fail");
}
