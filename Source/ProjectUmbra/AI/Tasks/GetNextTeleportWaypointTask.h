// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "GetNextTeleportWaypointTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UGetNextTeleportWaypointTask : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UGetNextTeleportWaypointTask();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Patrol Loc Key")
		FBlackboardKeySelector m_FPatrolLocKey;

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;
};
