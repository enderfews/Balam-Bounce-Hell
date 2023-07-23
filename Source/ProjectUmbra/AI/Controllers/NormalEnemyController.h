// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "NormalEnemyController.generated.h"


class ANormalEnemy;
class UAIPerceptionComponent;
class UBlackboardComponent;
class UBlackboardData;
class UBehaviorTree;
class UAISenseConfig_Sight;

UCLASS()
class PROJECTUMBRA_API ANormalEnemyController : public AAIController
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UBlackboardData* m_pBB;
	UPROPERTY()
	UBehaviorTree* m_pBT;
	UPROPERTY()
	UAISenseConfig_Sight* m_USightConfig = nullptr;

	bool m_bIsInAggro = false;
public:
	ANormalEnemyController();

	UPROPERTY(VisibleAnywhere,BlueprintReadWrite,Category="Components",DisplayName="AIPerception")
		UAIPerceptionComponent* mc_pAIPerception;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "EnemyRef")
		ANormalEnemy* m_pEnemyRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Blackboard")
		UBlackboardComponent* mc_pBlackboard;
protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
