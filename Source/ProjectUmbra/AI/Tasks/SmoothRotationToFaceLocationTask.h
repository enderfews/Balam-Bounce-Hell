// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SmoothRotationToFaceLocationTask.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API USmoothRotationToFaceLocationTask : public UBTTaskNode
{
	GENERATED_BODY()
private:
	UBehaviorTreeComponent* m_pBTC;
public:
	USmoothRotationToFaceLocationTask();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Location to face")
		FBlackboardKeySelector m_FLocationToFace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Attributes", DisplayName = "Must finish rotation to continue execution")
		bool m_bFinishRotationToContinue;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
		void EndRotation();
	virtual FString GetStaticDescription() const override;
};
