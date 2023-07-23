// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/QueryGenerators/EnvQueryGenerator_LateralCones.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvironmentQuery/Contexts/EnvQueryContext_Querier.h"


#define LOCTEXT_NAMESPACE "EnvQueryGenerator"

UEnvQueryGenerator_LateralCones::UEnvQueryGenerator_LateralCones():m_bUseQuerierRightVector(false)
{
	m_TQuerier = UEnvQueryContext_Querier::StaticClass();
	CenterActor = UEnvQueryContext_Querier::StaticClass();
	AlignedPointsDistance.DefaultValue = 100.f;
	ConeDegrees.DefaultValue = 90.f;
	AngleStep.DefaultValue = 10.f;
	Range.DefaultValue = 1000.f;
	//bIncludeContextLocation = false;
}

void UEnvQueryGenerator_LateralCones::BindDataToDataProviders(FEnvQueryInstance& QueryInstance) const
{
	UObject* BindOwner = QueryInstance.Owner.Get();
	AlignedPointsDistance.BindData(BindOwner, QueryInstance.QueryID);
	ConeDegrees.BindData(BindOwner, QueryInstance.QueryID);
	AngleStep.BindData(BindOwner, QueryInstance.QueryID);
	Range.BindData(BindOwner, QueryInstance.QueryID);
}

void UEnvQueryGenerator_LateralCones::GenerateItems(FEnvQueryInstance& QueryInstance) const
{
	const float ConeDegreesValue = FMath::Clamp(FMath::Abs(ConeDegrees.GetValue()), 0.f, 359.f);
	if (ConeDegreesValue == 0)
	{
		return;
	}
	TArray<AActor*> CenterActors;
	QueryInstance.PrepareContext(CenterActor, CenterActors);

	if (CenterActors.Num() <= 0)
	{
		return;
	}
	TArray<AActor*> Querier;
	if (m_bUseQuerierRightVector)
	{
		QueryInstance.PrepareContext(m_TQuerier, Querier);
	}

	BindDataToDataProviders(QueryInstance);

	const float AlignedPointDistanceValue = AlignedPointsDistance.GetValue();
	const float AngleStepValue = FMath::Clamp(AngleStep.GetValue(), 1.f, 359.f);
	const float GenerationRange = FMath::Clamp(Range.GetValue(), 0.f, MAX_flt);
	const int32 MaxPointsPerAngleValue = FMath::FloorToInt(GenerationRange / AlignedPointDistanceValue);

	TArray<FNavLocation> GeneratedItems;
	GeneratedItems.Reserve((CenterActors.Num() * FMath::CeilToInt(ConeDegreesValue / AngleStepValue) * MaxPointsPerAngleValue + 1)*2);
	FVector vRightVector, vLeftVector, ActorLocation;
	for (int32 CenterIndex = 0; CenterIndex < CenterActors.Num(); CenterIndex++)
	{
		//const FVector ForwardVector = CenterActors[CenterIndex]->GetActorForwardVector();
		if (m_bUseQuerierRightVector)
		{
			vRightVector = Querier[0]->GetActorRightVector();
			vLeftVector = Querier[0]->GetActorRightVector() * -1.f;
		}
		else {
			vRightVector = CenterActors[CenterIndex]->GetActorRightVector();
			vLeftVector = CenterActors[CenterIndex]->GetActorRightVector() * -1.f;
		}
		ActorLocation = CenterActors[CenterIndex]->GetActorLocation();
		for (float Angle = -(ConeDegreesValue * 0.5f); Angle < (ConeDegreesValue * 0.5f); Angle += AngleStepValue)
		{
			//const FVector AxisDirection = ForwardVector.RotateAngleAxis(Angle, FVector::UpVector);
			const FVector vLeftAxisDirection = vLeftVector.RotateAngleAxis(Angle, FVector::UpVector);
			const FVector vRightAxisDirection = vRightVector.RotateAngleAxis(Angle, FVector::UpVector);
			// skipping PointIndex == 0 as that's the context's location
			for (int32 PointIndex = 1; PointIndex <= MaxPointsPerAngleValue; PointIndex++)
			{
				const FVector vLeftGeneratedLocation = ActorLocation + (vLeftAxisDirection * PointIndex * AlignedPointDistanceValue);
				const FVector vRightGeneratedLocation = ActorLocation + (vRightAxisDirection * PointIndex * AlignedPointDistanceValue);
				GeneratedItems.Add(FNavLocation(vLeftGeneratedLocation));
				GeneratedItems.Add(FNavLocation(vRightGeneratedLocation));
			}
		}
	}

	ProjectAndFilterNavPoints(GeneratedItems, QueryInstance);
	StoreNavPoints(GeneratedItems, QueryInstance);
}

FText UEnvQueryGenerator_LateralCones::GetDescriptionTitle() const
{
	return FText::Format(LOCTEXT("LateralConesDescriptionGenerateAroundContext", "{0}: generate in laterals of {1}"),
		Super::GetDescriptionTitle(), UEnvQueryTypes::DescribeContext(CenterActor));
}

FText UEnvQueryGenerator_LateralCones::GetDescriptionDetails() const
{
	return FText::FromString("No description");
}


#undef LOCTEXT_NAMESPACE