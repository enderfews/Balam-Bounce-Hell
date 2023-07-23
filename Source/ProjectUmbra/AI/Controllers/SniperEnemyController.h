// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "SniperEnemyController.generated.h"



class ASniperEnemy;
class APlayerCharacter;
class UBlackboardData;
class UBehaviorTree;
class UAISenseConfig_Sight;
class UAIPerceptionComponent;

UCLASS()
class PROJECTUMBRA_API ASniperEnemyController : public AAIController
{
	GENERATED_BODY()

private:
	UBlackboardData* m_pBB;
	UBehaviorTree* m_pBT;
	UAISenseConfig_Sight* m_USightConfig = nullptr;
public:

	ASniperEnemyController();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller refs", DisplayName = "Enemy Reference")
		ASniperEnemy* m_pSniperRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Controller refs", DisplayName = "Enemy Reference")
		APlayerCharacter* m_pPlayerRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Blackboard")
		UBlackboardComponent* mc_pBlackboard;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "AIPerception")
		UAIPerceptionComponent* mc_pAIPerception;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void BeginPlay() override;
public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
};
