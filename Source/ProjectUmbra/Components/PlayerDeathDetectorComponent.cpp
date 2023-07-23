// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDeathDetectorComponent.h"

#include "HealthComponetCpp.h"
#include "Kismet/GameplayStatics.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"


void UPlayerDeathDetectorComponent::PlayerDeath()
{
	if(OnPlayerDeath.IsBound())
	{
		OnPlayerDeath.Broadcast();
	}
}

void UPlayerDeathDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	if(m_TObservedPlayer == nullptr)
	{
		m_TObservedPlayer = Cast<APlayerCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), APlayerCharacter::StaticClass()));
	}
	
	if(IsValid(m_TObservedPlayer))
	{
		m_TObservedPlayer->mc_UHealth->OnDeath.AddDynamic(this, &UPlayerDeathDetectorComponent::PlayerDeath);
	}
}

UPlayerDeathDetectorComponent::UPlayerDeathDetectorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
