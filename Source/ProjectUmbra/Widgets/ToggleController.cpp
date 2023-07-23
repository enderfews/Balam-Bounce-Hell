// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectUmbra/Widgets/ToggleController.h"
#include "Components/TextBlock.h"

void UToggleController::NativeConstruct()
{
	Super::NativeConstruct();
}

void UToggleController::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UToggleController::ControllerToggleWarning()
{
	if (m_bUseControllerUI)
	{
		m_pText->SetText(FText::FromString("Using controller"));
	}
	else
	{
		m_pText->SetText(FText::FromString("Using mouse and keyboard"));
	}
}
