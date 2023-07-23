// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Widgets/HealthBar.h"
#include "Components/ProgressBar.h"

float UHealthBar::GetHealth()
{
	return m_fCurrentHealth/m_fMaxHealth;
}

FColor UHealthBar::GetColor()
{
	if (m_bIsThePlayer)
	{
		return FColor::Green;
	}
	else
	{
		return FColor::Red;
	}
}

void UHealthBar::NativeConstruct()
{
	Super::NativeConstruct();
}

void UHealthBar::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	ProgressBar_70->SetPercent(GetHealth());
}
