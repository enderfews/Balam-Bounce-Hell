// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_LastSeenLoc.generated.h"


struct FEnvQueryContextData;
struct FEnvQueryInstance;

UCLASS()
class PROJECTUMBRA_API UEnvQueryContext_LastSeenLoc : public UEnvQueryContext
{
	GENERATED_BODY()

public:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
};
