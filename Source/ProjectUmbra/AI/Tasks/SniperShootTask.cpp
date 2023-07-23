// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/SniperShootTask.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"


FString USniperShootTask::GetStaticDescription() const
{
	return FString("Order the sniper to shoot");
}
EBTNodeResult::Type USniperShootTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ASniperEnemyController* pController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	if (pController)
	{
		ASniperEnemy* pEnemyRef = pController->m_pSniperRef;
		if (pEnemyRef)
		{
			pEnemyRef->ShootBullet();
			return EBTNodeResult::Succeeded;
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Sniper shoot task cpp: Could not get the normal enemy ref"));
			return EBTNodeResult::Failed;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Sniper shoot task cpp: Could not get the controller"));
		return EBTNodeResult::Failed;
	}

}
