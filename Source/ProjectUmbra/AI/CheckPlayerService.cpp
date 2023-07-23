// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/CheckPlayerService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"

void UCheckPlayerService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
	AActor* pPlayerKey = Cast<AActor>(pBlackboard->GetValueAsObject(m_FPlayerKey.SelectedKeyName));
	ANormalEnemyController* pController = Cast<ANormalEnemyController>(OwnerComp.GetOwner());
	ANormalEnemy* pEnemy = pController ? pController->m_pEnemyRef : nullptr;

	if (pEnemy->m_bIsAbortedByCheckpoint)
	{
		if (pBlackboard)
		{
			pBlackboard->ClearValue(FName("Player"));
		}
		//pEnemy->m_bIsAbortedByCheckpoint = false;
	}
	//*
	if (!pPlayerKey)
	{
		if (pEnemy)
		{
			TArray<TEnumAsByte<EObjectTypeQuery>> TQueries;
			TArray<AActor*> TToIgnore;
			TToIgnore.Add(pEnemy);
			TArray<AActor*> TOutActors;
			TQueries.Add(EObjectTypeQuery::ObjectTypeQuery2);
			UKismetSystemLibrary::SphereOverlapActors(GetWorld(), pEnemy->GetActorLocation(),400.f,TQueries, nullptr, TToIgnore, TOutActors);
			APlayerCharacter* pPlayer = nullptr;
			FHitResult Hit;
			FVector vDist;
			bool HasHit = false;
			for (AActor* pActor : TOutActors)
			{
				pPlayer = Cast<APlayerCharacter>(pActor);
				if (pPlayer)
				{
					TToIgnore.Empty(2);
					//TToIgnore.Add(pPlayer);
					TToIgnore.Add(pEnemy);
					vDist = (pPlayer->GetActorLocation() - pEnemy->GetActorLocation());
					vDist.Normalize();
					HasHit=UKismetSystemLibrary::LineTraceSingle(GetWorld(), pEnemy->GetActorLocation(), pEnemy->GetActorLocation()+(vDist*pEnemy->m_fPursuitRange), ETraceTypeQuery::TraceTypeQuery2, true, TToIgnore, EDrawDebugTrace::None, Hit, true);
					if (HasHit)
					{
						if (Cast<APlayerCharacter>(Hit.Actor))
						{
							pBlackboard->SetValueAsObject(m_FPlayerKey.SelectedKeyName, pPlayer);
							pEnemy->GetCharacterMovement()->MaxWalkSpeed = pEnemy->m_fChasingSpeed;
							break;
						}
					}
				}
			}
		}
	} /**/
}
