// Fill out your copyright notice in the Description page of Project Settings.


#include "HaloShooterEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "UObject/ConstructorHelpers.h"
#include "ProjectUmbra/Enums/EnemyType.h"
#include "ProjectUmbra/Characters/HaloShooterEnemy.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "Perception/AISenseConfig_Sight.h"

AHaloShooterEnemyController::AHaloShooterEnemyController()
{
	m_USightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	m_USightConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_USightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	m_USightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	m_USightConfig->PeripheralVisionAngleDegrees = 180.f;
	m_USightConfig->AutoSuccessRangeFromLastSeenLocation = 1.f;
	m_USightConfig->SetMaxAge(0.f);
	mc_pAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	mc_pAIPerception->ConfigureSense(*m_USightConfig);
	mc_pAIPerception->SetDominantSense(m_USightConfig->GetSenseImplementation());
	static ConstructorHelpers::FObjectFinder<UBlackboardData> SearchBB(TEXT("BlackboardData'/Game/IA/Blackboards/HaloShooterEnemyBB.HaloShooterEnemyBB'"));
	if (SearchBB.Succeeded())
	{
		m_pBB = SearchBB.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Halo Shooter Enemy Controller: Couldn't find the blackboard"));
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SearchBT(TEXT("BehaviorTree'/Game/IA/BehaviorTrees/HaloShooterEnemyBT1.HaloShooterEnemyBT1'"));
	if (SearchBT.Succeeded())
	{
		m_pBT = SearchBT.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Halo Shooter Enemy Controller: Couldn't find the behavior tree"));

	}
	mc_pAIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &AHaloShooterEnemyController::OnPerceptionUpdated);
}

void AHaloShooterEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	AHaloShooterEnemy* pEnemy = Cast<AHaloShooterEnemy>(InPawn);
	if (pEnemy)
	{
		m_pEnemyRef = pEnemy;
		UseBlackboard(m_pBB, mc_pBlackboard);
		//m_USightConfig->SightRadius = m_pEnemyRef->m_fDetectionRange;
		//m_USightConfig->LoseSightRadius = m_pEnemyRef->m_fDetectionRange + 200.f;
		if (mc_pBlackboard)
		{
			mc_pBlackboard->SetValueAsEnum(FName("Enemy"), (uint8)m_pEnemyRef->m_eEnemyType);
			mc_pBlackboard->SetValueAsFloat(FName("ShootDelay"), m_pEnemyRef->m_fShootDelay);
			mc_pBlackboard->SetValueAsFloat(FName("IdleTime"), m_pEnemyRef->m_fIdleTime);
			mc_pBlackboard->SetValueAsEnum(FName("MoveMode"), static_cast<uint8>(m_pEnemyRef->m_eMoveMode));
			mc_pBlackboard->SetValueAsInt(FName("WaypointIndex"), 1);
			mc_pBlackboard->SetValueAsVector(FName("GuardLoc"), pEnemy->GetActorLocation());
			RunBehaviorTree(m_pBT);

		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Halo Shooter Enemy Controller: Couldn't use the blackboard"));
		}

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Halo Shooter Enemy Controller: Couldn't cast the possessed pawn into normal enemy"));
	}
}

void AHaloShooterEnemyController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APlayerCharacter* pPlayer = Cast<APlayerCharacter>(Actor);
	if (pPlayer)
	{
		TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);
		if (SenseClass == UAISense_Sight::StaticClass())
		{
			AActor* pActor = Cast<AActor>(mc_pBlackboard->GetValueAsObject(FName("Player")));
			if (pActor)
			{
				UE_LOG(LogTemp, Error, TEXT("PLAYER CLEANED BY HALO SHOOTER"));
				mc_pBlackboard->ClearValue(FName("Player"));
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("PLAYER DETECTED BY HALO SHOOTER"));
				mc_pBlackboard->SetValueAsObject(FName("Player"), pPlayer);
			}
		}
	}
}
