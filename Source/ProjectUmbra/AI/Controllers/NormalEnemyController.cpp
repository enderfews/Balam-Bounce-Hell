// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UObject/ConstructorHelpers.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/SoundManager.h"

ANormalEnemyController::ANormalEnemyController()
{
	PrimaryActorTick.bCanEverTick = true;
	m_USightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	m_USightConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_USightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	m_USightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	m_USightConfig->PeripheralVisionAngleDegrees = 180.f;
	m_USightConfig->AutoSuccessRangeFromLastSeenLocation = 0.0f;
	m_USightConfig->SetMaxAge(0.0f);
	mc_pAIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	static ConstructorHelpers::FObjectFinder<UBlackboardData> SearchBB(TEXT("BlackboardData'/Game/IA/Blackboards/EnemyCppBB.EnemyCppBB'"));
	if (SearchBB.Succeeded())
	{
		m_pBB = SearchBB.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal Enemy Controller: Couldn't find the blackboard"));
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SearchBT(TEXT("BehaviorTree'/Game/IA/BehaviorTrees/NormalEnemyCppTree.NormalEnemyCppTree'"));
	if (SearchBT.Succeeded())
	{
		m_pBT = SearchBT.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal Enemy Controller: Couldn't find the behavior tree"));

	}
	mc_pAIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ANormalEnemyController::OnPerceptionUpdated);
}

void ANormalEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ANormalEnemy* pEnemy = Cast<ANormalEnemy>(InPawn);
	if (pEnemy)
	{
		m_pEnemyRef = pEnemy;
		m_USightConfig->SightRadius = m_pEnemyRef->m_fCombatRange;
		m_USightConfig->LoseSightRadius = m_pEnemyRef->m_fPursuitRange;

		mc_pAIPerception->ConfigureSense(*m_USightConfig);
		mc_pAIPerception->SetDominantSense(m_USightConfig->GetSenseImplementation());
		
		UseBlackboard(m_pBB, mc_pBlackboard);
		if (mc_pBlackboard)
		{
			mc_pBlackboard->SetValueAsFloat(FName("AimTime"), m_pEnemyRef->m_fAimTime);
			mc_pBlackboard->SetValueAsFloat(FName("ShootingRate"), m_pEnemyRef->m_fShootingRate);
			mc_pBlackboard->SetValueAsFloat(FName("ShootingRange"), m_pEnemyRef->m_fShootingRange);
			mc_pBlackboard->SetValueAsFloat(FName("PunchRange"), m_pEnemyRef->m_fPunchRange);
			mc_pBlackboard->SetValueAsFloat(FName("AreaCD"), m_pEnemyRef->m_fAreaCD);
			mc_pBlackboard->SetValueAsFloat(FName("PursuitRange"), m_pEnemyRef->m_fPursuitRange);
			mc_pBlackboard->SetValueAsFloat(FName("CombatRange"), m_pEnemyRef->m_fCombatRange);
			mc_pBlackboard->SetValueAsEnum(FName("Enemy"),static_cast<uint8>(m_pEnemyRef->m_eEnemyType));
			mc_pBlackboard->SetValueAsEnum(FName("MoveMode"), static_cast<uint8>(m_pEnemyRef->m_eMoveMode));
			mc_pBlackboard->SetValueAsInt(FName("WaypointIndex"),1);
			mc_pBlackboard->SetValueAsVector(FName("GuardLoc"),pEnemy->GetActorLocation());
			mc_pBlackboard->SetValueAsFloat(FName("WaypointPause"), m_pEnemyRef->m_fWaypointPause);
			mc_pBlackboard->SetValueAsBool(FName("PredictLocToShoot"), m_pEnemyRef->m_bPredictLocToShoot);
			mc_pBlackboard->SetValueAsBool(FName("ChargeAttackUsed"), false);
			mc_pBlackboard->SetValueAsBool(FName("IsEnemyAtLimit"), m_pEnemyRef->m_bIsEnemyAtLimit);
			mc_pBlackboard->SetValueAsFloat(FName("ChargeAimDuration"), m_pEnemyRef->m_fChargeAimDuration);
			mc_pBlackboard->SetValueAsBool(FName("CanUseChargeAttack"), m_pEnemyRef->mc_pHealthComp->m_fMaxHealth > 1.f);
			mc_pBlackboard->SetValueAsFloat(FName("ChargeRestDuration"), m_pEnemyRef->m_fChargeRestDuration);
			mc_pBlackboard->SetValueAsFloat(FName("PrepareChargeDuration"), m_pEnemyRef->m_fPrepareChargeDuration);
			mc_pBlackboard->SetValueAsFloat(FName("MinDistanceBetweenPlayer"), m_pEnemyRef->m_fMinDistanceBetweenPlayer);
			//mc_pBlackboard->SetValueAsBool(FName("LimitedByItsArea"), m_pEnemyRef->m_bLimitedByItsArea);
			if (m_pBT)
			{
				RunBehaviorTree(m_pBT);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Error in Normal Enemy Controller: Couldn't use the behavior tree"));
			}

		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Normal Enemy Controller: Couldn't use the blackboard"));
		}

	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal Enemy Controller: Couldn't cast the possessed pawn into normal enemy"));
	}
}

void ANormalEnemyController::OnUnPossess()
{
	Super::OnUnPossess();

	if(m_bIsInAggro)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Combat Ended"));
		GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager()->RemoveAggro();
		m_bIsInAggro = false;
	}
}

void ANormalEnemyController::BeginPlay()
{
	Super::BeginPlay();
}

void ANormalEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANormalEnemyController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(Actor);
	if (Player)
	{
		TSubclassOf<UAISense> SenseClass= UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), Stimulus);
		if (SenseClass == UAISense_Sight::StaticClass())
		{
			if (!Stimulus.WasSuccessfullySensed())
			{
				mc_pBlackboard->SetValueAsVector(FName("LastSeenLoc"), Player->GetActorLocation());
				mc_pBlackboard->ClearValue(FName("Player"));
				m_pEnemyRef->GetCharacterMovement()->MaxWalkSpeed=m_pEnemyRef->m_fPatrolSpeed;
			}
			else {
				mc_pBlackboard->SetValueAsObject(FName("Player"), Player);
				m_pEnemyRef->GetCharacterMovement()->MaxWalkSpeed = m_pEnemyRef->m_fChasingSpeed;
			}
			
			UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Change On Perception"));
			if(Stimulus.WasSuccessfullySensed() && !m_bIsInAggro)
			{
				UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Combat Started"));
				GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager()->AddAggro();
				m_bIsInAggro = true;
			}
			else if(!Stimulus.WasSuccessfullySensed() && m_bIsInAggro)
			{
				UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Combat Ended"));
				GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager()->RemoveAggro();
				m_bIsInAggro = false;
			}
		}
	}
}
