// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "ChargeAttackTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UChargeAttackTask : public UBTTaskNode
{
	GENERATED_BODY()
private:
	UBehaviorTreeComponent* m_pBTC;
public:
	UChargeAttackTask();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Entity to charge key")
		FBlackboardKeySelector m_FEntityToCharge;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UFUNCTION()
		void ChargeEnded();
};
