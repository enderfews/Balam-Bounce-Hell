// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GetNextWaypointTask.generated.h"

class UBehaviorTreeComponent;


UCLASS()
class PROJECTUMBRA_API UGetNextWaypointTask : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UGetNextWaypointTask();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Patrol Loc Key")
		FBlackboardKeySelector m_FPatrolLocKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
