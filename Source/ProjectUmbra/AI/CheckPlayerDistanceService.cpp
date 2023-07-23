// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/CheckPlayerDistanceService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "ProjectUmbra/Enums/EnemyCombatState.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"

void UCheckPlayerDistanceService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	AActor* pPlayer = Cast<AActor>(pBlackboard->GetValueAsObject(m_FPlayerKey.SelectedKeyName));
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	if (pBlackboard)
	{
		if (pController)
		{
			ANormalEnemy* pEnemy = pController->m_pEnemyRef;
			if (pEnemy)
			{
				if (pEnemy->m_bCanUseChargeAttack && pEnemy->mc_pHealthComp->m_fHealth == 1.f)
				{
					pBlackboard->SetValueAsBool(FName("IsEnemyAtLimit"), true);
				}
				if (pPlayer)
				{
					float Length = (pPlayer->GetActorLocation() - pEnemy->GetActorLocation()).Size();
					if (Length > pBlackboard->GetValueAsFloat(m_FPursuitRangeKey.SelectedKeyName)) {

						pBlackboard->SetValueAsVector(m_FLastSeenLoc.SelectedKeyName, pPlayer->GetActorLocation());
						pBlackboard->ClearValue(m_FPlayerKey.SelectedKeyName);
					}
					else if ((Length <= pBlackboard->GetValueAsFloat(m_FPursuitRangeKey.SelectedKeyName)) && (Length > pBlackboard->GetValueAsFloat(m_FCombatRangeKey.SelectedKeyName)))
					{
						pBlackboard->SetValueAsEnum(m_FCombatStateKey.SelectedKeyName, static_cast<uint8>(EEnemyCombatState::OnPursuitRange));
					}else if ((Length <= pBlackboard->GetValueAsFloat(m_FCombatRangeKey.SelectedKeyName)) && (Length > pBlackboard->GetValueAsFloat(m_FMinDistanceBetweenPlayerKey.SelectedKeyName)))
					{
						pBlackboard->SetValueAsEnum(m_FCombatStateKey.SelectedKeyName, static_cast<uint8>(EEnemyCombatState::OnShootingRange));
					}
					else if ((Length <= pBlackboard->GetValueAsFloat(m_FMinDistanceBetweenPlayerKey.SelectedKeyName)) && (Length > pBlackboard->GetValueAsFloat(m_FPunchRangeKey.SelectedKeyName)))
					{
						pBlackboard->SetValueAsEnum(m_FCombatStateKey.SelectedKeyName, static_cast<uint8>(EEnemyCombatState::OnStayBack));
					}
					else if (Length <= pBlackboard->GetValueAsFloat(m_FPunchRangeKey.SelectedKeyName))
					{
						pBlackboard->SetValueAsEnum(m_FCombatStateKey.SelectedKeyName, static_cast<uint8>(EEnemyCombatState::OnPunchRange));
					}
				}
				
			}
		}
	}
}
