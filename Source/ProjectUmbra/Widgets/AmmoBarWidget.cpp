// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoBarWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"

void UAmmoBarWidget::SetMaxAmmo(int _iMaxAmmo)
{
	m_TAmmoElements.Empty();
	for(int32 i = 0; i<_iMaxAmmo; ++i)
	{
		UImage* NewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("AmmoElement"+i));
		NewImage->SetColorAndOpacity(FLinearColor(1.0f,1.0f,1.0f,0.0f));
		m_TAmmoElements.Add(NewImage);
		UHorizontalBoxSlot* NewImageSlot = i%2 == 0 ?
			m_HorizontalBoxUpper->AddChildToHorizontalBox(NewImage) :
			m_HorizontalBoxLower->AddChildToHorizontalBox(NewImage);
		NewImageSlot->Size = FSlateChildSize(ESlateSizeRule::Automatic);
		NewImageSlot->HorizontalAlignment = HAlign_Left;
		NewImageSlot->VerticalAlignment = VAlign_Center;
		if(i%2 == 0)
		{
			NewImageSlot->Padding.Right = 32;
		} else
		{
			NewImageSlot->Padding.Left = 32;
		}
	}
}

void UAmmoBarWidget::SetCurrentAmmo(int _iCurrentAmmo)
{
	for(int32 i=m_TAmmoElements.Num()-1; i>=_iCurrentAmmo; --i)
	{
		m_TAmmoElements[i]->SetColorAndOpacity(FLinearColor(1.0f,1.0f,1.0f, 0.0f));
	}
}

void UAmmoBarWidget::ChangeColor(FLinearColor _FColor)
{
	for(int32 i=0; i<m_TAmmoElements.Num(); ++i)
	{
		m_TAmmoElements[i]->SetColorAndOpacity(_FColor);
	}
}
