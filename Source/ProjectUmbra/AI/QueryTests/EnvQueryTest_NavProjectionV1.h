// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvironmentQuery/EnvQueryTest.h"
#include "EnvQueryTest_NavProjectionV1.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UEnvQueryTest_NavProjectionV1 : public UEnvQueryTest
{
	GENERATED_BODY()
public:

	UEnvQueryTest_NavProjectionV1();

	/** trace params */
	UPROPERTY(EditDefaultsOnly, Category = Test)
		FEnvTraceData ProjectionData;

	virtual void RunTest(FEnvQueryInstance& QueryInstance) const override;
	virtual FText GetDescriptionDetails() const override;
};
