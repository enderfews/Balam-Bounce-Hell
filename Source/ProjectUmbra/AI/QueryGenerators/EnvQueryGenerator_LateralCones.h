// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryGenerator.h"
#include "EnvironmentQuery/Generators/EnvQueryGenerator_ProjectedPoints.h"
#include "DataProviders/AIDataProvider.h"
#include "EnvQueryGenerator_LateralCones.generated.h"


class UEnvQueryContext;


UCLASS(meta = (DisplayName = "Lateral cones"))
class PROJECTUMBRA_API UEnvQueryGenerator_LateralCones : public UEnvQueryGenerator_ProjectedPoints
{
	GENERATED_BODY()
public:

	UEnvQueryGenerator_LateralCones();

	/** Binds data to data providers
 *  @param QueryInstance - the instance of the query
 */
	void BindDataToDataProviders(FEnvQueryInstance& QueryInstance) const;

	virtual void GenerateItems(FEnvQueryInstance& QueryInstance) const override;

	/** Returns the title of the generator on the corresponding node in the EQS Editor window */
	virtual FText GetDescriptionTitle() const override;

	/** Returns the details of the generator on the corresponding node in the EQS Editor window */
	virtual FText GetDescriptionDetails() const override;

	UPROPERTY(EditAnywhere, Category = Generator)
		TSubclassOf<UEnvQueryContext> CenterActor;
private:
	TSubclassOf<UEnvQueryContext> m_TQuerier;
protected:
	/** Distance between each point of the same angle */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
		FAIDataProviderFloatValue AlignedPointsDistance;

	/** Maximum degrees of the generated cone */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
		FAIDataProviderFloatValue ConeDegrees;

	/** The step of the angle increase. Angle step must be >=1
	 *  Smaller values generate less items
	 */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
		FAIDataProviderFloatValue AngleStep;

	/** Generation distance */
	UPROPERTY(EditDefaultsOnly, Category = Generator)
		FAIDataProviderFloatValue Range;
	UPROPERTY(EditDefaultsOnly, Category = Generator, DisplayName = "Use querier right vector")
		bool m_bUseQuerierRightVector;
};
