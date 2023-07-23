// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/SmoothRotationToFaceTask.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"



USmoothRotationToFaceTask::USmoothRotationToFaceTask():m_bFinishRotationToContinue(false)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}
FString USmoothRotationToFaceTask::GetStaticDescription() const
{
	return FString("Perform a smooth rotation to face the target entity");
}
EBTNodeResult::Type USmoothRotationToFaceTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	m_pBTC = &OwnerComp;
	if (pController)
	{
		ANormalEnemy* pEnemy = pController->m_pEnemyRef;
		if (pEnemy)
		{
			UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
			AActor* pEntityToFace=Cast<AActor>(pBlackboard->GetValueAsObject(m_FEntityToFace.SelectedKeyName));
			if (pEntityToFace)
			{
				FVector vEnemyLoc = pEnemy->GetActorLocation();
				vEnemyLoc.Z = 0.f;
				FVector vTargetLoc = pEntityToFace->GetActorLocation();
				vTargetLoc.Z = 0.f;
				FRotator rTarget = UKismetMathLibrary::FindLookAtRotation(vEnemyLoc,vTargetLoc);
				FRotator rCurrent = pEnemy->GetActorRotation();
				if (m_bFinishRotationToContinue)
				{
					pEnemy->m_FOnRotationFaceEnded.AddDynamic(this, &USmoothRotationToFaceTask::EndRotation);
					pEnemy->SmoothFacePlayer(rCurrent, rTarget);
					return EBTNodeResult::InProgress;
				}
				else {
					pEnemy->SmoothFacePlayer(rCurrent, rTarget);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}
	return EBTNodeResult::Failed;
}

void USmoothRotationToFaceTask::EndRotation()
{
	FinishLatentTask(*m_pBTC, EBTNodeResult::Succeeded);
}
