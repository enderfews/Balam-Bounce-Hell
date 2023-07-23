// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Components/Image.h"

void UHealthBarWidget::SetMaxHealth(int _iMaxHealth)
{
	UTexture2D* Texture = LoadObject<UTexture2D>(nullptr, TEXT("Texture2D'/Game/Textures/Icons/Heart.Heart'"), nullptr, LOAD_None, nullptr);

#if WITH_EDITORONLY_DATA
	Texture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
#endif
	Texture->LODGroup = TextureGroup::TEXTUREGROUP_UI;
	Texture->UpdateResource();

	for(int32 i = 0; i < _iMaxHealth; ++i)
	{
		UImage* NewImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Hearth"+i));
		NewImage->SetBrushFromTexture(Texture);
		NewImage->SetBrushSize(FVector2D(64,64));
		m_UHearths.Add(NewImage);
		UHorizontalBoxSlot* NewImageSlot = m_HorizontalBox->AddChildToHorizontalBox(NewImage);
		NewImageSlot->Padding.Right = 10.0f;
		NewImageSlot->HorizontalAlignment = HAlign_Left;
		NewImageSlot->VerticalAlignment = VAlign_Center;
	}
}

void UHealthBarWidget::SetCurrentHealth(int _iCurrentHealth)
{
	for(int32 i = m_UHearths.Num()-1; i>=_iCurrentHealth; --i)
	{
		m_UHearths[i]->SetOpacity(0.0f);
	}

	for(int32 i = 0; i<_iCurrentHealth; ++i)
	{
		m_UHearths[i]->SetOpacity(1.0f);
	}
}
