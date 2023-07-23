// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveEnemyController.h"

#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "ProjectUmbra/Characters/ExplosiveEnemy.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BehaviorTree/BehaviorTree.h"

AExplosiveEnemyController::AExplosiveEnemyController()
{
	//Creation of perception component and Sight Config
	mc_UAIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AI Perception"));
	m_USightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));

	//Set the Sight detection booleans to detect everything (without this the enemy does not see the player)
	m_USightConfig->DetectionByAffiliation.bDetectEnemies = true;
	m_USightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	m_USightConfig->DetectionByAffiliation.bDetectFriendlies = true;

	m_USightConfig->SetMaxAge(0.2f);

	//Add the Sight Config to the perception component and sets the Sight as the primary sense
	mc_UAIPerceptionComponent->ConfigureSense(*m_USightConfig);
	mc_UAIPerceptionComponent->SetDominantSense(m_USightConfig->GetSenseImplementation());

	//Gets the Assets for configuration of AIController in the future
	static ConstructorHelpers::FObjectFinder<UBlackboardData>BlackboardAsset(TEXT("BlackboardData'/Game/IA/Blackboards/ExplosiveEnemyBB.ExplosiveEnemyBB'"));
	m_UBlackboardAsset = BlackboardAsset.Object;
	
	static ConstructorHelpers::FObjectFinder<UBehaviorTree>BehaviourTreeAsset(TEXT("BehaviorTree'/Game/IA/BehaviorTrees/ExplosiveEnemyBT1.ExplosiveEnemyBT1'"));
	m_UBehaviorTreeAsset = BehaviourTreeAsset.Object;
}

void AExplosiveEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	//Grabs reference to the specific Enemy Class, if the Enemy is of another class simply returns
	const AExplosiveEnemy* AExplosiveEnemyCharacter = Cast<AExplosiveEnemy>(InPawn);

	if(AExplosiveEnemyCharacter == nullptr)
	{
		return;
	}

	//Configures Sight Config radius and half angle of vision with the Enemy variables
	m_USightConfig->PeripheralVisionAngleDegrees = 180.0f;
	m_USightConfig->SightRadius = AExplosiveEnemyCharacter->m_fDetectionRange;
	m_USightConfig->LoseSightRadius = AExplosiveEnemyCharacter->m_fDetectionRange;
	mc_UAIPerceptionComponent->ConfigureSense(*m_USightConfig);
	m_pExplosiveRef = const_cast<AExplosiveEnemy*>(AExplosiveEnemyCharacter);

	//Assigns the BlackBoard to the AIController and store a reference to the component, then configures the blackboard values
	UseBlackboard(m_UBlackboardAsset,m_UBlackboardComponent);
	
	m_UBlackboardComponent->SetValueAsFloat(TEXT("DetectionRange"),AExplosiveEnemyCharacter->m_fDetectionRange);
	m_UBlackboardComponent->SetValueAsFloat(TEXT("ExplosionRange"),AExplosiveEnemyCharacter->m_fExplosionDetectionRange);
	m_UBlackboardComponent->SetValueAsVector(FName("StartPosition"), AExplosiveEnemyCharacter->GetActorLocation());
	m_UBlackboardComponent->SetValueAsBool(FName("IsUnderground"), true);
	m_UBlackboardComponent->SetValueAsBool(FName("Explode"), false);

	//Starts the BehaviourTree and setup the callback for seeing the player
	RunBehaviorTree(m_UBehaviorTreeAsset);

	mc_UAIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AExplosiveEnemyController::TargetPerceptionUpdated);
}

void AExplosiveEnemyController::TargetPerceptionUpdated(AActor* _AActor, FAIStimulus _FAIStimulus)
{
	//Tests if the Actor triggering the perception is the player
	
	if(UGameplayStatics::GetPlayerPawn(GetWorld(),0) != _AActor)
	{
		return;
	}

	//Tests if the Stimuli received is of type Sight

	if(UAIPerceptionSystem::GetSenseClassForStimulus(GetWorld(), _FAIStimulus) != UAISense_Sight::StaticClass())
	{
		return;
	}

	//Sets the player on the Blackboard
	
	if(_FAIStimulus.WasSuccessfullySensed())
	{
		m_UBlackboardComponent->SetValueAsObject(TEXT("Player"), _AActor);
		m_pExplosiveRef->GetCharacterMovement()->MaxWalkSpeed = m_pExplosiveRef->m_fChasingSpeed;
	}
}
