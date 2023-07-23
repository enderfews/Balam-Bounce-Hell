// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_EnemyStartLoc.generated.h"

struct FEnvQueryContextData;
struct FEnvQueryInstance;
/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UEnvQueryContext_EnemyStartLoc : public UEnvQueryContext
{
	GENERATED_BODY()

	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
