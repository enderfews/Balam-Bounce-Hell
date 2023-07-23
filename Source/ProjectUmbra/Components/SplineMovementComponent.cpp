// Fill out your copyright notice in the Description page of Project Settings.


#include "SplineMovementComponent.h"

#include "TimerManager.h"
#include "Components/SplineComponent.h"
#include "ProjectUmbra/Characters/SplinePath.h"


// Sets default values for this component's properties
USplineMovementComponent::USplineMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


void USplineMovementComponent::EnableTick()
{
	SetComponentTickEnabled(true);
}

void USplineMovementComponent::DisabledTick()
{
	SetComponentTickEnabled(false);
}

void USplineMovementComponent::Start()
{
	if(m_FStopTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(m_FStopTimerHandle);
	}
	if(m_fStopTimeAtStart > 0.0f)
	{
		DisabledTick();
		GetWorld()->GetTimerManager().SetTimer(m_FStopTimerHandle, this, &USplineMovementComponent::EnableTick, m_fStopTimeAtStart, false);
	}
	SetComponentTickEnabled(true);
}

void USplineMovementComponent::Stop()
{
	if(m_FStopTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(m_FStopTimerHandle);
	}
	SetComponentTickEnabled(false);
}

void USplineMovementComponent::ReverseMovementDirection()
{
	m_fMovementDirection *= -1;
}

// Called when the game starts
void USplineMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	m_fCurrentStopTimeAtStart = m_fStopTimeAtStart;

	if(!IsValid(m_APathToFollow))
	{
		Stop();
		return;
	}
	
	m_fCurrentDistance = m_APathToFollow->mc_USplineComponent->GetSplineLength() * m_fStartingProgress;
	const FVector vTrainLocation = m_APathToFollow->mc_USplineComponent->GetLocationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);
	const FRotator rTrainRotation = m_APathToFollow->mc_USplineComponent->GetRotationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);

	GetOwner()->SetActorLocationAndRotation(vTrainLocation, rTrainRotation);

	if(!m_bStartEnabled)
	{
		DisabledTick();
	}
	else if(m_fStopTimeAtStart > 0.0f)
	{
		DisabledTick();
		GetWorld()->GetTimerManager().SetTimer(m_FStopTimerHandle, this, &USplineMovementComponent::EnableTick, m_fStopTimeAtStart, false);
	}
}

#if WITH_EDITOR
void USplineMovementComponent::PostEditChangeProperty(FPropertyChangedEvent& _FPropertyChangedEvent)
{
	Super::PostEditChangeProperty(_FPropertyChangedEvent);

	if(GET_MEMBER_NAME_CHECKED(USplineMovementComponent, m_EMovementType) == _FPropertyChangedEvent.GetPropertyName())
	{
		if(!IsValid(m_APathToFollow))
		{
			Stop();
			return;
		}
		m_APathToFollow->mc_USplineComponent->SetClosedLoop(m_EMovementType == ESplineMovementType::Loop);
	}

	if(GET_MEMBER_NAME_CHECKED(USplineMovementComponent, m_APathToFollow) == _FPropertyChangedEvent.GetPropertyName())
	{
		if(!IsValid(m_APathToFollow))
		{
			return;
		}
		m_fCurrentDistance = m_APathToFollow->mc_USplineComponent->GetSplineLength() * m_fStartingProgress;
		const FVector vTrainLocation = m_APathToFollow->mc_USplineComponent->GetLocationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);
		const FRotator rTrainRotation = m_APathToFollow->mc_USplineComponent->GetRotationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);

		GetOwner()->SetActorLocationAndRotation(vTrainLocation, rTrainRotation);
	}

	if(GET_MEMBER_NAME_CHECKED(USplineMovementComponent, m_fStartingProgress) == _FPropertyChangedEvent.GetPropertyName())
	{
		if(!IsValid(m_APathToFollow))
		{
			Stop();
			return;
		}
		
		m_fCurrentDistance = m_APathToFollow->mc_USplineComponent->GetSplineLength() * m_fStartingProgress;
		const FVector vTrainLocation = m_APathToFollow->mc_USplineComponent->GetLocationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);
		const FRotator rTrainRotation = m_APathToFollow->mc_USplineComponent->GetRotationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);

		GetOwner()->SetActorLocationAndRotation(vTrainLocation, rTrainRotation);
	}
}
#endif

// Called every frame
void USplineMovementComponent::TickComponent(float _fDeltaTime, ELevelTick _ETickType,
                                             FActorComponentTickFunction* _FThisTickFunction)
{
	Super::TickComponent(_fDeltaTime, _ETickType, _FThisTickFunction);

	if(!IsValid(m_APathToFollow))
	{
		Stop();
		return;
	}

	//Move distance using speed
	m_fCurrentDistance = m_fCurrentDistance + m_fMovementDirection * m_fSpeed * _fDeltaTime;

	//Calculate new position if at max distance or bellow zero based on movement type
	if(m_EMovementType == ESplineMovementType::Loop)
	{
		if(m_fCurrentDistance >= m_APathToFollow->mc_USplineComponent->GetSplineLength())
		{
			if(m_fStopTimeAtEnd == 0.0f && m_fStopTimeAtStart == 0.0f)
			{
				m_fCurrentDistance -= m_APathToFollow->mc_USplineComponent->GetSplineLength();
			} else
			{
				m_fCurrentDistance = 0;
				
				DisabledTick();
				GetWorld()->GetTimerManager().SetTimer(m_FStopTimerHandle, this, &USplineMovementComponent::EnableTick, m_fStopTimeAtEnd+m_fStopTimeAtStart, false);
			}
		}
	}

	if(m_EMovementType == ESplineMovementType::Once)
	{
		if(m_fCurrentDistance >= m_APathToFollow->mc_USplineComponent->GetSplineLength())
		{
			m_fCurrentDistance = m_APathToFollow->mc_USplineComponent->GetSplineLength();
			DisabledTick();
		}

		if(m_fCurrentDistance <= 0)
		{
			m_fCurrentDistance = 0;
			DisabledTick();
		}
	}

	if(m_EMovementType == ESplineMovementType::PingPong)
	{
		if(m_fCurrentDistance >= m_APathToFollow->mc_USplineComponent->GetSplineLength())
		{
			if(m_fStopTimeAtEnd == 0.0f && m_fStopTimeAtStart == 0.0f)
			{
				m_fCurrentDistance -= m_APathToFollow->mc_USplineComponent->GetSplineLength();
				m_fCurrentDistance = m_APathToFollow->mc_USplineComponent->GetSplineLength() - m_fCurrentDistance;
				
				ReverseMovementDirection();
			} else
			{
				m_fCurrentDistance = m_APathToFollow->mc_USplineComponent->GetSplineLength();
				
				ReverseMovementDirection();
				DisabledTick();
				GetWorld()->GetTimerManager().SetTimer(m_FStopTimerHandle, this, &USplineMovementComponent::EnableTick, m_fStopTimeAtEnd, false);
			}
			
			
		} else if(m_fCurrentDistance <= 0)
		{
			if(m_fStopTimeAtEnd == 0.0f && m_fStopTimeAtStart == 0.0f)
			{
				m_fCurrentDistance = -m_fCurrentDistance;
				
				ReverseMovementDirection();
			}
			else
			{
				m_fCurrentDistance = 0;

				ReverseMovementDirection();
				DisabledTick();
				GetWorld()->GetTimerManager().SetTimer(m_FStopTimerHandle, this, &USplineMovementComponent::EnableTick, m_fStopTimeAtStart, false);
			}
			
		}
	}

	//Get World Location and Rotation from the spline based on distance and set position adn rotation of train root
	const FVector vTrainLocation = m_APathToFollow->mc_USplineComponent->GetLocationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);
	const FRotator rTrainRotation = m_APathToFollow->mc_USplineComponent->GetRotationAtDistanceAlongSpline(m_fCurrentDistance, ESplineCoordinateSpace::World);

	GetOwner()->SetActorLocationAndRotation(vTrainLocation, rTrainRotation);
}

