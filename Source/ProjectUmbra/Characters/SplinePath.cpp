// Fill out your copyright notice in the Description page of Project Settings.


#include "SplinePath.h"

#include "Components/SplineComponent.h"


// Sets default values
ASplinePath::ASplinePath()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	mc_USplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
	SetRootComponent(mc_USplineComponent);
}
