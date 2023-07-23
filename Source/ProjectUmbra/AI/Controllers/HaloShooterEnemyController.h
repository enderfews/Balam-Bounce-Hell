// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "HaloShooterEnemyController.generated.h"

class AHaloShooterEnemy;
class UAIPerceptionComponent;
class UBlackboardComponent;
class UBlackboardData;
class UBehaviorTree;
class UAISenseConfig_Sight;

UCLASS()
class PROJECTUMBRA_API AHaloShooterEnemyController : public AAIController
{
	GENERATED_BODY()

private:
	UBlackboardData* m_pBB;
	UBehaviorTree* m_pBT;
	UPROPERTY()
		UAISenseConfig_Sight* m_USightConfig = nullptr;
public:
	AHaloShooterEnemyController();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "AIPerception")
		UAIPerceptionComponent* mc_pAIPerception;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "EnemyRef")
		AHaloShooterEnemy* m_pEnemyRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "AIPerception")
		UBlackboardComponent* mc_pBlackboard;

protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION()
		void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
