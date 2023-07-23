// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/AI/SmartObjects/SplineMovementCurve.h"
#include "ProjectUmbra/Characters/NormalEnemy.h"
#include "Components/SplineComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Components/CapsuleComponent.h"
// Sets default values
ASplineMovementCurve::ASplineMovementCurve():m_pEnemyRef(nullptr), m_bAttackMode(false)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	mc_UCurve = CreateDefaultSubobject<USplineComponent>(TEXT("Curve movement"));
	mc_UCurve->SetupAttachment(GetRootComponent());
	//mc_UCurve->bAllowDiscontinuousSpline = true;
	ConstructorHelpers::FObjectFinder<UCurveFloat> FindCurve(TEXT("CurveFloat'/Game/Blueprints/Curves/Platform_Curves/Platform_Curve.Platform_Curve'"));
	if (FindCurve.Succeeded())
	{
		m_pCurve = FindCurve.Object;
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error in SplineMovementCurve: Could not find the Curve float"));
	}
}

void ASplineMovementCurve::MoveAlongSplineFunc(float alpha)
{
	if (m_pEnemyRef)
	{
		float fDistance=UKismetMathLibrary::Lerp(0.f, mc_UCurve->GetSplineLength(), alpha);
		FVector vLoc = mc_UCurve->GetLocationAtDistanceAlongSpline(fDistance, ESplineCoordinateSpace::World);
		FRotator rRot = mc_UCurve->GetRotationAtDistanceAlongSpline(fDistance, ESplineCoordinateSpace::World);
		rRot.Roll = 0.f;
		rRot.Pitch = 0.f;
		vLoc.Z+=m_pEnemyRef->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		m_pEnemyRef->SetActorLocation(vLoc);
		if (!m_bAttackMode)
		{
			m_pEnemyRef->SetActorRotation(rRot);
		}
	}
}

void ASplineMovementCurve::OnPathEnded()
{
	if (m_OnSplineEnded.IsBound())
	{
		m_OnSplineEnded.Broadcast(this);
	}
	mc_UCurve->ClearSplinePoints(true);
}

// Called when the game starts or when spawned
void ASplineMovementCurve::BeginPlay()
{
	Super::BeginPlay();
	if (m_pCurve)
	{
		FOnTimelineFloat OnTimelineFloat;
		OnTimelineFloat.BindUFunction(this, FName("MoveAlongSplineFunc"));
		m_TLMoveAlongSpline.AddInterpFloat(m_pCurve,OnTimelineFloat);
		FOnTimelineEvent OnTimelineEvent;
		OnTimelineEvent.BindUFunction(this, FName("OnPathEnded"));
		m_TLMoveAlongSpline.SetTimelineFinishedFunc(OnTimelineEvent);
	}
}

// Called every frame
void ASplineMovementCurve::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	m_TLMoveAlongSpline.TickTimeline(DeltaTime);
}

void ASplineMovementCurve::PerformSmoothMove(ANormalEnemy* pEnemy, float fEnemySpeed)
{
	m_pEnemyRef = pEnemy;
	float fTimelineRate =mc_UCurve->GetSplineLength() / fEnemySpeed;
	m_TLMoveAlongSpline.SetPlayRate(1.f/fTimelineRate);
	m_TLMoveAlongSpline.PlayFromStart();
}

void ASplineMovementCurve::AddPointsToSpline(TArray<FVector>& _TPoints)
{
	//FSplinePoint FPoint;
	//mc_UCurve->AddSplinePoint(_TPoints[0], ESplineCoordinateSpace::World, true);
	//mc_UCurve->AddPoint(FPoint, true);
	//for (int i=1; i<_TPoints.Num();i++)
	//{
		//mc_UCurve->AddSplinePoint(_TPoints[i], ESplineCoordinateSpace::World, false);
		//FPoint.Position = _TPoints[i];
		//FPoint.Scale = FVector::OneVector;
		//FPoint.Rotation = UKismetMathLibrary::FindLookAtRotation(_TPoints[i-1], _TPoints[i]);
		//FPoint.ArriveTangent = _TPoints[i] - _TPoints[i-1];
		//FPoint.LeaveTangent = FVector::ZeroVector;
		//mc_UCurve->AddPoint(FPoint, true);
	//}
	//First we add all the points to the spline
	//First we eliminate unnecessary points
	bool bStartRemove = false;
	FHitResult Hit;
	for (int i = 0; i < _TPoints.Num(); ++i)
	{
		FVector vAct = _TPoints[i];
		for (int j = _TPoints.Num() - 1; j != i; --j)
		{
			if(bStartRemove)
			{
				_TPoints.RemoveAt(j);
			}
			else if (!UKismetSystemLibrary::LineTraceSingle(GetWorld(), vAct, _TPoints[j], ETraceTypeQuery::TraceTypeQuery1, false, TArray<AActor*>(), EDrawDebugTrace::None, Hit, true))
			{
				bStartRemove = true;
				continue;
			}
		}
		bStartRemove = false;
	}
	for (FVector vPoint : _TPoints)
	{
		mc_UCurve->AddSplinePoint(vPoint, ESplineCoordinateSpace::World, true);
	}
	//Then we reduce the tangents of the splines
	/*FVector vTangent;
	for (int i = 0; i < mc_UCurve->GetNumberOfSplinePoints(); ++i)
	{
		vTangent = mc_UCurve->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		vTangent /= 2.f;
		mc_UCurve->SetTangentAtSplinePoint(i, vTangent, ESplineCoordinateSpace::World, true);
	}*/
}

