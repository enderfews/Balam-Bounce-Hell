// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/QueryContext/EnvQueryContext_LastSeenLoc.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "ProjectUmbra/AI/Controllers/NormalEnemyController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"

void UEnvQueryContext_LastSeenLoc::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);

	ANormalEnemy* pEnemy = Cast<ANormalEnemy>(QueryInstance.Owner.Get());
	ANormalEnemyController* pController = pEnemy ? Cast<ANormalEnemyController>(UAIBlueprintHelperLibrary::GetAIController(pEnemy)) : nullptr;
	UBlackboardComponent* pBB = pController ? pController->GetBlackboardComponent() : nullptr;
	if (pBB)
	{
		FVector vLastSeenLoc(pBB->GetValueAsVector(FName("LastSeenLoc")));
		UEnvQueryItemType_Point::SetContextHelper(ContextData, vLastSeenLoc);
	}
	else {
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("Error in EnvQueryContext_LastSeenLoc: Could not get the blackboard component"));
		return;
	}
}