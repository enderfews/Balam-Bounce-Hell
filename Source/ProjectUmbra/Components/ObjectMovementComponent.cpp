#include "ObjectMovementComponent.h"

#include "GameFramework/Actor.h"

void UObjectMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	this->SetComponentTickEnabled(false);
	m_VWorldStartLocation = GetOwner()->GetActorLocation();
	m_VWorldEndLocation = GetOwner()->GetActorLocation() + m_VEndLocation;
}

UObjectMovementComponent::UObjectMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UObjectMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	m_fProgress = FMath::Clamp(m_fProgress + m_iDirection * (DeltaTime / m_fTimeToMove),0.0f,1.0f);
	GetOwner()->SetActorLocation(FMath::LerpStable(m_VWorldStartLocation, m_VWorldEndLocation, m_fProgress));
	if(m_fProgress == 0.0f)
	{
		if(OnMovementReverseEnd.IsBound())
			OnMovementReverseEnd.Broadcast();
		Stop();
	} 
	if(m_fProgress == 1.0f)
	{
		if(OnMovementEnd.IsBound())
			OnMovementEnd.Broadcast();
		Stop();
	}
}

void UObjectMovementComponent::Start()
{
	m_iDirection = 1.0f;
	if(OnMovementStart.IsBound())
		OnMovementStart.Broadcast();
	this->SetComponentTickEnabled(true);
}

void UObjectMovementComponent::Stop()
{
	this->SetComponentTickEnabled(false);
}

void UObjectMovementComponent::Reverse()
{
	m_iDirection = -1.0f;
	if(OnMovementReverseStart.IsBound())
		OnMovementReverseStart.Broadcast();
	this->SetComponentTickEnabled(true);
}
