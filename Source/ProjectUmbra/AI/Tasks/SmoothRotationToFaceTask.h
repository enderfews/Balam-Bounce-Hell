// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "SmoothRotationToFaceTask.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class PROJECTUMBRA_API USmoothRotationToFaceTask : public UBTTaskNode
{
	GENERATED_BODY()
private:
	UBehaviorTreeComponent* m_pBTC;
public:
	USmoothRotationToFaceTask();
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Entity to face")
		FBlackboardKeySelector m_FEntityToFace;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Task Attributes", DisplayName = "Must finish rotation to continue execution")
		bool m_bFinishRotationToContinue;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
		void EndRotation();
	virtual FString GetStaticDescription() const override;
};
