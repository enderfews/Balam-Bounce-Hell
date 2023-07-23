// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Controllers/SniperEnemyController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "ProjectUmbra/Enums/EnemyType.h"
#include "ProjectUmbra/Enums/SniperTeleportPattern.h"
#include "ProjectUmbra/Characters/SniperEnemy.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"

ASniperEnemyController::ASniperEnemyController() :m_pSniperRef(nullptr),m_pPlayerRef(nullptr),mc_pBlackboard(nullptr)
,m_pBB(nullptr),m_pBT(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;
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
	static ConstructorHelpers::FObjectFinder<UBlackboardData> SearchBB(TEXT("BlackboardData'/Game/IA/Blackboards/EnemyCppBB.EnemyCppBB'"));
	if (SearchBB.Succeeded())
	{
		m_pBB = SearchBB.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal Enemy Controller: Couldn't find the blackboard"));
	}
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> SearchBT(TEXT("BehaviorTree'/Game/IA/BehaviorTrees/SniperEnemyTree.SniperEnemyTree'"));
	if (SearchBT.Succeeded())
	{
		m_pBT = SearchBT.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in Normal Enemy Controller: Couldn't find the behavior tree"));

	}
	mc_pAIPerception->OnTargetPerceptionUpdated.AddDynamic(this, &ASniperEnemyController::OnPerceptionUpdated);
}

void ASniperEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ASniperEnemy* pSniper = Cast<ASniperEnemy>(InPawn);
	if (pSniper)
	{
		m_pSniperRef = pSniper;
		m_USightConfig->SightRadius = m_pSniperRef->m_fDetectionRange;
		m_USightConfig->LoseSightRadius = m_pSniperRef->m_fDetectionRange+200.f;
		UseBlackboard(m_pBB, mc_pBlackboard);
		if (mc_pBlackboard)
		{
			mc_pBlackboard->SetValueAsEnum(FName("Enemy"), static_cast<uint8>(EEnemyType::Sniper));
			mc_pBlackboard->SetValueAsFloat(FName("ChargeTime"), m_pSniperRef->m_fChargeTime);
			mc_pBlackboard->SetValueAsFloat(FName("SniperCooldown"), m_pSniperRef->m_fCooldown);
			mc_pBlackboard->SetValueAsFloat(FName("TimeUntilTeleport"), m_pSniperRef->m_fTimeUntilTeleport);
			mc_pBlackboard->SetValueAsFloat(FName("MaxTeleportCount"), static_cast<float>(m_pSniperRef->m_iMaxTeleportCount));
			mc_pBlackboard->SetValueAsInt(FName("WaypointIndex"), 1);
			mc_pBlackboard->SetValueAsEnum(FName("TeleportPattern"), static_cast<uint8>(m_pSniperRef->m_eTeleportPattern));
			mc_pBlackboard->SetValueAsFloat(FName("TimeAfterAiming"), m_pSniperRef->m_fTimeAfterAiming);
			mc_pBlackboard->SetValueAsBool(FName("KeepsTeleportLocation"), m_pSniperRef->m_bKeepsTeleportLocation);
			if (m_pBT)
			{
				RunBehaviorTree(m_pBT);
			}
			else {
				UE_LOG(LogTemp, Error, TEXT("Error in Sniper Enemy Controller: Couldn't use the behavior tree"));
			}
		}
		else {
			UE_LOG(LogTemp, Error, TEXT("Error in Sniper Enemy Controller: Couldn't use the blackboard"));
		}
	}
	UE_LOG(LogTemp, Error, TEXT("Sniper possessed successfully"));
}

void ASniperEnemyController::BeginPlay()
{
	Super::BeginPlay();
}

void ASniperEnemyController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASniperEnemyController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
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
				mc_pBlackboard->ClearValue(FName("Player"));
				//mc_pBlackboard->SetValueAsObject(FName("Player"), nullptr);
			}
			else
			{
				mc_pBlackboard->SetValueAsObject(FName("Player"), pPlayer);
			}
		}
	}
}
