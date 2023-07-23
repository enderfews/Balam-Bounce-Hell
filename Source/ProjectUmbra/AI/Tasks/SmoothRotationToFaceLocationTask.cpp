// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/SmoothRotationToFaceLocationTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

USmoothRotationToFaceLocationTask::USmoothRotationToFaceLocationTask():m_bFinishRotationToContinue(false)
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}
EBTNodeResult::Type USmoothRotationToFaceLocationTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	m_pBTC = &OwnerComp;
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	ANormalEnemy* pEnemy = pController ? pController->m_pEnemyRef : nullptr;
	if (pEnemy)
	{
		FVector vLocToFace = pBlackboard->GetValueAsVector(m_FLocationToFace.SelectedKeyName);
		vLocToFace.Z = 0.f;
		FVector vCurrentLoc = pEnemy->GetActorLocation();
		vCurrentLoc.Z = 0.f;
		FRotator rTarget = UKismetMathLibrary::FindLookAtRotation(vCurrentLoc, vLocToFace);
		FRotator rCurrent = pEnemy->GetActorRotation();
		if (m_bFinishRotationToContinue)
		{
			if (!pEnemy->m_FOnRotationFaceEnded.IsAlreadyBound(this, &USmoothRotationToFaceLocationTask::EndRotation))
			{
				pEnemy->m_FOnRotationFaceEnded.AddDynamic(this, &USmoothRotationToFaceLocationTask::EndRotation);
			}
			pEnemy->SmoothFacePlayer(rCurrent, rTarget);
			return EBTNodeResult::InProgress;
		}
		else {
			pEnemy->SmoothFacePlayer(rCurrent, rTarget);
			return EBTNodeResult::Succeeded;
		}
	}
	else {
		#ifdef WITH_EDITOR
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in smooth rotation to face location: COULD NOT GET ENEMY REFERENCE"));
		#endif 
		UE_LOG(LogTemp, Error, TEXT("Error in smooth rotation to face location: COULD NOT GET ENEMY REFERENCE"));
		return EBTNodeResult::Failed;
	}
}
void USmoothRotationToFaceLocationTask::EndRotation()
{
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(m_pBTC->GetOwner());
	ANormalEnemy* pEnemy = pController ? pController->m_pEnemyRef : nullptr;
	if (pEnemy)
	{
		pEnemy->m_FOnRotationFaceEnded.RemoveDynamic(this, &USmoothRotationToFaceLocationTask::EndRotation);
	}
	FinishLatentTask(*m_pBTC, EBTNodeResult::Succeeded);
}
FString USmoothRotationToFaceLocationTask::GetStaticDescription() const
{
	return FString("Performs a rotation to face the next location, makes the movement more realistic");
}
