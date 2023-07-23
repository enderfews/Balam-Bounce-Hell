// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "TimerManager.h"
#include "AimAndChargeSniperTask.generated.h"


class UBehaviorTreeComponent;
class UAudioComponent;

UCLASS()
class PROJECTUMBRA_API UAimAndChargeSniperTask : public UBTTaskNode
{
	GENERATED_BODY()

private:
	FTimerHandle m_FTimer;
	UBehaviorTreeComponent* m_pBTC;
public:
	UAimAndChargeSniperTask();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Entity to face key")
		FBlackboardKeySelector m_FEntityToFace;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Charge time key")
		FBlackboardKeySelector m_FChargeTimeKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	UFUNCTION()
	void OnDelayFinished();
};
