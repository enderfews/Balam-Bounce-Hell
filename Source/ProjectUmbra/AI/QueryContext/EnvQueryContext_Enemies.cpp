// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/QueryContext/EnvQueryContext_Enemies.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Enemies::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	Super::ProvideContext(QueryInstance, ContextData);
	
	TArray<AActor*> TEnemies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), TEnemies);
	if (TEnemies.Num() == 0)
	{
		return;
	}
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, TEnemies);
}