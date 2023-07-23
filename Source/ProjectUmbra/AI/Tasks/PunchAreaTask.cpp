// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/PunchAreaTask.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
EBTNodeResult::Type UPunchAreaTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	if (pController)
	{
		ANormalEnemy* pEnemy=pController->m_pEnemyRef;
		if (pEnemy) {
			pEnemy->Punch();
			return EBTNodeResult::Succeeded;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Punch area task cpp: Could not get the normal enemy ref"));
			return EBTNodeResult::Failed;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Punch area task cpp: Could not get Controller of normal enemy"));
		return EBTNodeResult::Failed;
	}
}

FString UPunchAreaTask::GetStaticDescription() const
{
	return FString("Order the Enemy to perform a punch attack");
}
