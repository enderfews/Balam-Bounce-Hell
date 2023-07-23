// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/NormalShootTask.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"

UNormalShootTask::UNormalShootTask()
{
	bNotifyTick = true;
	bNotifyTaskFinished = true;
	bCreateNodeInstance = true;
}
EBTNodeResult::Type UNormalShootTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);
	m_bIsAborting = false;
	m_iBurstCount = 0;
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	if (pController)
	{
		ANormalEnemy* pEnemy = pController->m_pEnemyRef;
		if (pEnemy)
		{
			if (!pEnemy->m_bIsAbortedByCheckpoint)
			{
				m_pEnemyRef = pEnemy;
				m_pBTC = &OwnerComp;
				GetWorld()->GetTimerManager().SetTimer(m_FTimer, this, &UNormalShootTask::ShootTimer, m_pEnemyRef->m_fFireRate, true);
				return EBTNodeResult::InProgress;
			}
			else {
				return EBTNodeResult::Aborted;
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Normal Shoot task cpp: Could not get the normal enemy ref"));
			return EBTNodeResult::Failed;
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal Shoot task cpp: Could not get the normal enemy controller"));
		return EBTNodeResult::Failed;
	}
	return EBTNodeResult::Type();
}

EBTNodeResult::Type UNormalShootTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::AbortTask(OwnerComp, NodeMemory);
	m_bIsAborting = true;
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	if (pController)
	{
		ANormalEnemy* pEnemy = pController->m_pEnemyRef;
		if (pEnemy)
		{
			if (m_iBurstCount < pEnemy->m_iBurstNum && !pEnemy->m_bIsAbortedByCheckpoint) {
				GetWorld()->GetTimerManager().SetTimer(m_FTimer, this, &UNormalShootTask::ShootTimer, m_pEnemyRef->m_fFireRate, true);
				return EBTNodeResult::InProgress;
			}
			else {
				if (pEnemy->m_bIsAbortedByCheckpoint)
				{
					pEnemy->m_bIsAbortedByCheckpoint = false;
					UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
					if (pBlackboard)
					{
						pBlackboard->ClearValue(FName("Player"));
					}

				}
				return EBTNodeResult::Aborted;
			}
		}
	}
	return EBTNodeResult::Aborted;
}

FString UNormalShootTask::GetStaticDescription() const
{
	return FString();
}

void UNormalShootTask::ShootTimer()
{
	if (m_pEnemyRef)
	{
		if (m_pEnemyRef->m_bIsAbortedByCheckpoint)
		{
			m_pEnemyRef->m_bIsAbortedByCheckpoint = false;
			GetWorld()->GetTimerManager().ClearTimer(m_FTimer);
			UBehaviorTreeComponent* OwnerComp = m_pBTC;
			if (m_bIsAborting)
			{
				FinishLatentAbort(*OwnerComp);
			}
			else {
				FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
			}
		}
		else if (m_iBurstCount < m_pEnemyRef->m_iBurstNum)
		{
			UBlackboardComponent* pBlackboard= m_pBTC ? m_pBTC->GetBlackboardComponent() : nullptr;
			AActor* pPlayer=pBlackboard ? Cast<AActor>(pBlackboard->GetValueAsObject(FName("Player"))) : nullptr;
			ANormalEnemyController* pController = pPlayer ? Cast<ANormalEnemyController>(m_pBTC->GetOwner()): nullptr;
			if (pController)
			{
				float fDot = FVector::DotProduct(pController->m_pEnemyRef->GetActorForwardVector(), (pPlayer->GetActorLocation() - pController->m_pEnemyRef->GetActorLocation()).GetSafeNormal());
				float fCos=UKismetMathLibrary::DegAcos(fDot);
				if (fCos <= pController->m_pEnemyRef->m_fShootingAngle)
				{
					m_pEnemyRef->ShootBullet();
					m_iBurstCount++;
				}
				else {
					m_iBurstCount++;
				}
			}
			else {
				m_iBurstCount++;
			}
		}
		else {
			GetWorld()->GetTimerManager().ClearTimer(m_FTimer);
			UBehaviorTreeComponent* OwnerComp = m_pBTC;
			if (m_bIsAborting)
			{
				FinishLatentAbort(*OwnerComp);
			}
			else {
				FinishLatentTask(*OwnerComp, EBTNodeResult::Succeeded);
			}
		}
	}
}
