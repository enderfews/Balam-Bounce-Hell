// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/AimAndChargeSniperTask.h"
#include "Engine/World.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AudioComponent.h"

UAimAndChargeSniperTask::UAimAndChargeSniperTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}
EBTNodeResult::Type UAimAndChargeSniperTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);

	ASniperEnemyController* pEnemyController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pEnemyController ? pEnemyController->m_pSniperRef : nullptr;
	if (pEnemy)
	{
		pEnemy->StopAimSound();
		pEnemy->AbortLaser();
		return EBTNodeResult::Aborted;
	}
	else {
		return EBTNodeResult::Aborted;
	}
}
EBTNodeResult::Type UAimAndChargeSniperTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	m_pBTC = &OwnerComp;
	ASniperEnemyController* pEnemyController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pEnemyController? pEnemyController->m_pSniperRef :nullptr;
	UBlackboardComponent* pBlackboard = pEnemy? OwnerComp.GetBlackboardComponent(): nullptr;
	if (pBlackboard)
	{
		float fChargeTime = pBlackboard->GetValueAsFloat(m_FChargeTimeKey.SelectedKeyName);
		pEnemy->SetLaserAim(Cast<APlayerCharacter>(pBlackboard->GetValueAsObject(FName("Player"))));
		pEnemy->PlayTeleportSoundDelay(fChargeTime - pEnemy->m_fTimeOffsetAfterAimTeleportSoundStarts);
		GetWorld()->GetTimerManager().SetTimer(m_FTimer, this, &UAimAndChargeSniperTask::OnDelayFinished, fChargeTime, false);
		pEnemy->PlayAimSound();

		return EBTNodeResult::InProgress;
	}
	UE_LOG(LogTemp, Error, TEXT("Error in Aim and charge task cpp: Could not get the blackboard ref"));
	return EBTNodeResult::Failed;
}

void UAimAndChargeSniperTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	ASniperEnemyController* pEnemyController = Cast<ASniperEnemyController>(OwnerComp.GetOwner());
	ASniperEnemy* pEnemy = pEnemyController->m_pSniperRef;
	if (pBlackboard)
	{
		if (pEnemyController)
		{
			if (pEnemy)
			{
				FVector EnemyLoc = pEnemy->GetActorLocation();
				EnemyLoc.Z = 0.f;
				FVector TargetLoc = Cast<AActor>(pBlackboard->GetValueAsObject(m_FEntityToFace.SelectedKeyName))->GetActorLocation();
				TargetLoc.Z = 0.f;
				pEnemy->GetMesh()->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(EnemyLoc, TargetLoc));
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Error in Aim and charge task cpp: Could not get the enemy ref"));
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Aim and charge task cpp: Could not get the sniper controller ref"));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Aim and charge task cpp: Could not get the blackboard ref"));
	}
}

FString UAimAndChargeSniperTask::GetStaticDescription() const
{
	return FString("The sniper will aim for a while");
}

void UAimAndChargeSniperTask::OnDelayFinished()
{
	GetWorld()->GetTimerManager().ClearTimer(m_FTimer);
	UBehaviorTreeComponent* OwnerComp = m_pBTC;
	FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
}
