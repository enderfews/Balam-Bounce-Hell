// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SplinePath.generated.h"

class USplineComponent;

UCLASS()
class PROJECTUMBRA_API ASplinePath : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditInstanceOnly, DisplayName="Spline")
	USplineComponent* mc_USplineComponent;
	
	// Sets default values for this actor's properties
	ASplinePath();
};
	
