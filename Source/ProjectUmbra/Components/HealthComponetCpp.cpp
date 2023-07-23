// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Components/HealthComponetCpp.h"
#include "ProjectUmbra/Characters/PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "ProjectUmbra/System/UmbraGameMode.h"
#include "Kismet/KismetMathLibrary.h"
#include "ProjectUmbra/System/ProbabilityManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/Widgets/PlayerHUD.h"

// Sets default values for this component's properties
UHealthComponentCpp::UHealthComponentCpp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UHealthComponentCpp::BeginPlay()
{
	Super::BeginPlay();

	m_fHealth = m_fMaxHealth;
}


void UHealthComponentCpp::TakeDamage(float _Damage,bool _bFromDeathzone)
{
	if (!_bFromDeathzone &&(m_bGodMode || m_bImmune))
	{
		return;
	}
	
	m_fHealth = UKismetMathLibrary::Max(m_fHealth - _Damage, 0);

	m_bImmune = true;
	m_fTimeToResetImmunityTime = m_fImmunityTime;
	m_fTimeToChangeColor = m_fDamagedColorDuration;
	
	if (m_fHealth == 0.f)
	{
		if(OnDeath.IsBound())
		{
			OnDeath.Broadcast();
		}
	}
	else
	{
		if(OnDamaged.IsBound())
		{
			OnDamaged.Broadcast(m_fHealth);
		}
	}
}

void UHealthComponentCpp::HealDamage(float _HealAmount)
{
	const float fHealth = UKismetMathLibrary::Clamp((m_fHealth + _HealAmount), 0, m_fMaxHealth);
	if(fHealth > m_fHealth && OnHealed.IsBound())
	{
		OnHealed.Broadcast(fHealth);
	}
	m_fHealth = fHealth;
}


void UHealthComponentCpp::UpdateTimers(float _fDeltaTime)
{
	if (m_fTimeToResetImmunityTime > 0.f) m_fTimeToResetImmunityTime -= _fDeltaTime;
	else if(m_bImmune)
	{
		m_bImmune = false;
		if(OnImmunityEnd.IsBound())
		{
			OnImmunityEnd.Broadcast();
		}
	}

	if(m_fTimeToChangeColor > 0.0f) m_fTimeToChangeColor -= _fDeltaTime;
	else if(m_bImmune && OnDamagedEnd.IsBound())
	{
		OnDamagedEnd.Broadcast();
	}
}

// Called every frame
void UHealthComponentCpp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateTimers(DeltaTime);
}

void UHealthComponentCpp::SetHealth(float _fHealth)
{
	if(_fHealth > 0.0f && _fHealth <= m_fMaxHealth)
	{
		m_fHealth = _fHealth;

		if(OnHealthChanged.IsBound())
		{
			OnHealthChanged.Broadcast(m_fHealth);
		}
	}
}

float UHealthComponentCpp::GetHealth() const
{
	return m_fHealth;
}

bool UHealthComponentCpp::GetImmune() const
{
	return m_bImmune || m_bGodMode;
}

void UHealthComponentCpp::MakeInmmune()
{
	m_bImmune = true;
	m_fTimeToResetImmunityTime = m_fImmunityTime;
	m_fTimeToChangeColor = m_fDamagedColorDuration;
	if (OnDamaged.IsBound())
	{
		OnDamaged.Broadcast(m_fHealth);
	}
}
