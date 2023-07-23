// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TimerManager.h"
#include "NormalShootTask.generated.h"

/**
 * 
 */

class ANormalEnemy;
class UBehaviorTreeComponent;
UCLASS()
class PROJECTUMBRA_API UNormalShootTask : public UBTTaskNode
{
	GENERATED_BODY()
private:
	UBehaviorTreeComponent* m_pBTC;
public:
	UNormalShootTask();
	//Task Attributes
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Burst Count")
		int m_iBurstCount=0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Timer var")
		FTimerHandle m_FTimer;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Is Aborting")
		bool m_bIsAborting;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Task Attributes", DisplayName = "Enemy Ref")
		ANormalEnemy* m_pEnemyRef;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

	UFUNCTION()
	void ShootTimer();
};
