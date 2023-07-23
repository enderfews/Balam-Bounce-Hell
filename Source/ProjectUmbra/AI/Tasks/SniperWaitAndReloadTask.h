// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "TimerManager.h"
#include "SniperWaitAndReloadTask.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class PROJECTUMBRA_API USniperWaitAndReloadTask : public UBTTaskNode
{
	GENERATED_BODY()
private:
	FTimerHandle m_FTimer;
	UBehaviorTreeComponent* m_pBTC;
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Sniper cooldown key")
		FBlackboardKeySelector m_FSniperCooldownKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Teleport times key")
		FBlackboardKeySelector m_FTeleportTimesKey;

	USniperWaitAndReloadTask();

	UFUNCTION()
		void OnDelayFinished();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
