// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "CheckPlayerDistanceService.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UCheckPlayerDistanceService : public UBTService
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Shooting range key")
		FBlackboardKeySelector m_FShootingRangeKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Player key")
		FBlackboardKeySelector m_FPlayerKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Pursuit range key")
		FBlackboardKeySelector m_FPursuitRangeKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Combat state key")
		FBlackboardKeySelector m_FCombatStateKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Punch range key")
		FBlackboardKeySelector m_FPunchRangeKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Last seen Loc key")
		FBlackboardKeySelector m_FLastSeenLoc;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Combat range key")
		FBlackboardKeySelector m_FCombatRangeKey;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Service Attributes", DisplayName = "Min distance between player key")
		FBlackboardKeySelector m_FMinDistanceBetweenPlayerKey;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
