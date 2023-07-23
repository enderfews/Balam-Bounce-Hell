// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "CheckPlayerService.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UCheckPlayerService : public UBTService
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Player key")
		FBlackboardKeySelector m_FPlayerKey;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
