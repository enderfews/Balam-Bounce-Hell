// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "ExplosiveEnemyController.generated.h"

class UAISenseConfig_Sight;
class AExplosiveEnemy;
/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API AExplosiveEnemyController : public AAIController
{
	GENERATED_BODY()
private:
	//SighConfig used needed to configure the perception
	UPROPERTY(VisibleAnywhere, Category="Components", DisplayName="Sight Config")
	UAISenseConfig_Sight* m_USightConfig = nullptr;

	//Blackboard component used to access and set the blackboard values
	UPROPERTY(VisibleAnywhere, Category="Components", DisplayName="Blackboard Component")
	UBlackboardComponent* m_UBlackboardComponent = nullptr;

	//Asset references needed to configure the Blackboard and BehaviorTree
	UPROPERTY(VisibleAnywhere, Category="Components", DisplayName="Blackboard Component")
	UBlackboardData* m_UBlackboardAsset = nullptr;
	UPROPERTY(VisibleAnywhere, Category="Components", DisplayName="Blackboard Component")
	UBehaviorTree* m_UBehaviorTreeAsset = nullptr;
	
protected:
	//Used to Initialize the Blackboard values and start the Behaviourtree
	virtual void OnPossess(APawn* InPawn) override;

	//Callback for the perception system
	UFUNCTION()
	void TargetPerceptionUpdated(AActor* _AActor, FAIStimulus _FAIStimulus);

public:
	//Perception component configured with sight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="AI Perception")
	UAIPerceptionComponent* mc_UAIPerceptionComponent = nullptr;

	AExplosiveEnemyController();

	AExplosiveEnemy* m_pExplosiveRef;
};
