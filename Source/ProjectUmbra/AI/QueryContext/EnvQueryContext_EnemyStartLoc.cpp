// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_EnemyStartLoc.h"

#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Point.h"
#include "Kismet/KismetSystemLibrary.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"

void UEnvQueryContext_EnemyStartLoc::ProvideContext(FEnvQueryInstance& QueryInstance,
                                                    FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	
	ABaseEnemy* QueryOwner = Cast<ABaseEnemy>(QueryInstance.Owner.Get());
	if(QueryOwner == nullptr)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), TEXT("No valid query owner"));
		return;
	}
		
	UEnvQueryItemType_Point::SetContextHelper(ContextData, QueryOwner->m_vCenterPatrolLoc);
}
