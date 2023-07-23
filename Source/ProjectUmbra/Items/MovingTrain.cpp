// Fill out your copyright notice in the Description page of Project Settings.


#include "MovingTrain.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
AMovingTrain::AMovingTrain()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	mc_USceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	SetRootComponent(mc_USceneRoot);
	
	mc_UTrainRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Train Root"));
	mc_UTrainRoot->SetupAttachment(mc_USceneRoot);

	mc_UTrainMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Train Mesh"));
	mc_UTrainMesh->SetupAttachment(mc_UTrainRoot);

	mc_UPath = CreateDefaultSubobject<USplineComponent>(TEXT("Path"));
	mc_UPath->SetupAttachment(mc_USceneRoot);
}

#if WITH_EDITOR
void AMovingTrain::PostEditChangeProperty(FPropertyChangedEvent& _FPropertyChangedEvent)
{
	Super::PostEditChangeProperty(_FPropertyChangedEvent);

	if(GET_MEMBER_NAME_CHECKED(AMovingTrain, m_EMovementType) == _FPropertyChangedEvent.GetPropertyName())
	{
		mc_UPath->SetClosedLoop(m_EMovementType == EMovementType::Loop);
	}

	if(GET_MEMBER_NAME_CHECKED(AMovingTrain, m_fStartingProgress) == _FPropertyChangedEvent.GetPropertyName())
	{
		m_fCurrentDistance = mc_UPath->GetSplineLength() * m_fStartingProgress;
		const FVector vTrainLocation = mc_UPath->GetLocationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);
		const FRotator rTrainRotation = mc_UPath->GetRotationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);

		mc_UTrainRoot->SetWorldLocationAndRotation(vTrainLocation, rTrainRotation);
	}
}
#endif

void AMovingTrain::BeginPlay()
{
	Super::BeginPlay();

	m_fCurrentStopTimeAtStart = m_fStopTimeAtStart;
	
	m_fCurrentDistance = mc_UPath->GetSplineLength() * m_fStartingProgress;
	const FVector vTrainLocation = mc_UPath->GetLocationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);
	const FRotator rTrainRotation = mc_UPath->GetRotationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);

	mc_UTrainRoot->SetWorldLocationAndRotation(vTrainLocation, rTrainRotation);
}

void AMovingTrain::Tick(float _fDeltaTime)
{
	Super::Tick(_fDeltaTime);

	//Update stop timers
	if(m_fCurrentStopTimeAtEnd > 0)
	{
		m_fCurrentStopTimeAtEnd -= _fDeltaTime;
		return;
	}

	if(m_fCurrentStopTimeAtStart > 0)
	{
		m_fCurrentStopTimeAtStart -= _fDeltaTime;
		return;
	}

	//Restart movement if stop times are done
	if(m_bStop && m_fCurrentStopTimeAtEnd == 0 && m_fCurrentStopTimeAtStart == 0)
	{
		m_bStop = false;
	}

	//Don't calculate positions if object is stopped
	if(m_bStop)
	{
		return;
	}

	//Move distance using speed
	m_fCurrentDistance = m_fCurrentDistance + m_fMovementDirection * m_fSpeed * _fDeltaTime;

	//Calculate new position if at max distance or bellow zero based on movement type
	if(m_EMovementType == EMovementType::Loop)
	{
		if(m_fCurrentDistance >= mc_UPath->GetSplineLength())
		{
			if(m_fStopTimeAtEnd == 0.0f && m_fStopTimeAtStart == 0.0f)
			{
				m_fCurrentDistance -= mc_UPath->GetSplineLength();
			} else
			{
				m_fCurrentDistance = 0;
			}

			m_fCurrentStopTimeAtEnd = m_fStopTimeAtEnd;
			m_fCurrentStopTimeAtStart = m_fStopTimeAtStart;
		}
	}

	if(m_EMovementType == EMovementType::Once)
	{
		if(m_fCurrentDistance >= mc_UPath->GetSplineLength())
		{
			m_fCurrentDistance = mc_UPath->GetSplineLength();
			m_bStop = true;
			m_fCurrentStopTimeAtEnd = -1;
			m_fCurrentStopTimeAtStart = -1;
		}
	}

	if(m_EMovementType == EMovementType::PingPong)
	{
		if(m_fCurrentDistance >= mc_UPath->GetSplineLength())
		{
			if(m_fStopTimeAtEnd == 0.0f && m_fStopTimeAtStart == 0.0f)
			{
				m_fCurrentDistance -= mc_UPath->GetSplineLength();
				m_fCurrentDistance = mc_UPath->GetSplineLength() - m_fCurrentDistance;
				m_fMovementDirection = -1;
			} else
			{
				m_fCurrentDistance = mc_UPath->GetSplineLength();
				m_fMovementDirection = -1;
			}

			m_fCurrentStopTimeAtEnd = m_fStopTimeAtEnd;
			
		} else if(m_fCurrentDistance <= 0)
		{
			if(m_fStopTimeAtEnd == 0.0f && m_fStopTimeAtStart == 0.0f)
			{
				m_fCurrentDistance = -m_fCurrentDistance;
				m_fMovementDirection = 1;
			}
			else
			{
				m_fCurrentDistance = 0;
				m_fMovementDirection = 1;
			}

			m_fCurrentStopTimeAtStart = m_fStopTimeAtEnd;
		}
	}

	//Get World Location and Rotation from the spline based on distance and set position adn rotation of train root
	const FVector vTrainLocation = mc_UPath->GetLocationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);
	const FRotator rTrainRotation = mc_UPath->GetRotationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);

	mc_UTrainRoot->SetWorldLocationAndRotation(vTrainLocation, rTrainRotation);
	
}

