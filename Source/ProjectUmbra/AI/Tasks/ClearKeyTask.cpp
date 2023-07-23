// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/Tasks/ClearKeyTask.h"
#include "ClearKeyTask.h"
#include "BehaviorTree/BlackboardComponent.h"

FString UClearKeyTask::GetStaticDescription() const
{
    return FString("Clear the selected key in the blackboard");
}

EBTNodeResult::Type UClearKeyTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    Super::ExecuteTask(OwnerComp, NodeMemory);
    UBlackboardComponent* pBlackboard = OwnerComp.GetBlackboardComponent();
    if (pBlackboard)
    {
        pBlackboard->ClearValue(m_FKeyToClear.SelectedKeyName);
        return EBTNodeResult::Succeeded;
    }
    else {
        UE_LOG(LogTemp, Error, TEXT("Error in Clear key task: Could not find blackboard"));
        return EBTNodeResult::Failed;
    }
}
