// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "DisableSniperLaserTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UDisableSniperLaserTask : public UBTTaskNode
{
	GENERATED_BODY()

public:

	UDisableSniperLaserTask();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
