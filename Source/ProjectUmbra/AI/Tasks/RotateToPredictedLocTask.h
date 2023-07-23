// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "RotateToPredictedLocTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API URotateToPredictedLocTask : public UBTTaskNode
{
	GENERATED_BODY()
public:
	URotateToPredictedLocTask();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Entity to face")
		FBlackboardKeySelector m_FEntityToFace;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
