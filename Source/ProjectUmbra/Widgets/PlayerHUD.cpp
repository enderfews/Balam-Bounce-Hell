// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

// #include "AmmoBarWidget.h"
// #include "HealthBarWidget.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "Components/CanvasPanel.h"
#include "Components/SizeBox.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetTree.h"

void UPlayerHUD::SetAmmoStyle()
{
	m_UAmmoImage = WidgetTree->ConstructWidget<UImage>(UImage::StaticClass(), TEXT("Ammo"));
	m_UAmmoImage->SetBrush(NoAmmoBrush);

	m_UAmmoColor->AddChild(m_UAmmoImage);
}

void UPlayerHUD::SetMaxAmmo(int _iMaxAmmo)
{
	m_iMaxAmmo = _iMaxAmmo;
}

void UPlayerHUD::SetCurrentAmmo(int _iCurrentAmmo)
{
	// m_UAmmoBarWidget->SetPercent(_iCurrentAmmo/m_iMaxAmmo);
	m_UAmmoText->SetText(FText::AsNumber(_iCurrentAmmo));
	if (_iCurrentAmmo > 0 && m_UAmmoCanvas->GetVisibility() == ESlateVisibility::Hidden)
	{
		m_UAmmoCanvas->SetVisibility(ESlateVisibility::Visible);
		//m_UAmmoImage->SetBrush(AmmoBrush);
	}
	else if (_iCurrentAmmo <= 0 && m_UAmmoCanvas->GetVisibility() == ESlateVisibility::Visible)
	{
		m_UAmmoCanvas->SetVisibility(ESlateVisibility::Hidden);
		m_UAmmoImage->SetBrush(NoAmmoBrush);
	}
}

void UPlayerHUD::ChangeAmmoTag(EBulletModifier _ENewTag)
{
	switch (_ENewTag)
	{
		case EBulletModifier::Explosive:
		{
			m_UAmmoImage->SetBrush(ExplosiveAmmoBrush);
		} break;
		case EBulletModifier::Freezing:
		{
			m_UAmmoImage->SetBrush(FreezingAmmoBrush);
		} break;
		case EBulletModifier::Bouncy:
		{
			m_UAmmoImage->SetBrush(BouncyAmmoBrush);
		} break;
		default:
		{
	#ifdef WITH_EDITOR
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Error in Player tag: No color specified for current tag"));
	#endif 
			UE_LOG(LogTemp, Error, TEXT("Error in Player tag: No color specified for current tag"));
		}
	}
}

void UPlayerHUD::SetMaxHealth(int _iMaxHealth)
{
	// m_UHealthBarWidget->SetMaxHealth(_iMaxHealth);
	m_iMaxHealth = _iMaxHealth;
	m_iCurrentHealth = _iMaxHealth;
}

void UPlayerHUD::SetCurrentHealth(int _iCurrentHealth)
{
	// m_UHealthBarWidget->SetCurrentHealth(_iCurrentHealth);

	for (int32 i = m_UHealthCanvas->GetChildrenCount() - 1; i >= _iCurrentHealth; --i)
	{
		m_UHealthCanvas->GetChildAt(i)->SetRenderOpacity(0.0f);
	}

	for (int32 i = 0; i < _iCurrentHealth; ++i)
	{
		m_UHealthCanvas->GetChildAt(i)->SetRenderOpacity(1.0f);
	}
	
	if(m_iCurrentHealth > _iCurrentHealth)
	{
		float fOpacity = m_fOnHitOpacity;
		if(_iCurrentHealth <= m_iHealthToMaintainBorder)
		{
			const float fCurrentHealthOpacity = (float)1/(float)_iCurrentHealth;
			fOpacity += fCurrentHealthOpacity;
			m_fTargetOpacity = fCurrentHealthOpacity;
		} else
		{
			m_fTargetOpacity = 0.0f;
		}
		m_UDamageBorder->SetOpacity(fOpacity);
	} else
	{
		float fOpacity = 0.0f;
		if(_iCurrentHealth <= m_iHealthToMaintainBorder)
		{
			fOpacity = (float)1/(float)_iCurrentHealth;
			m_fTargetOpacity = fOpacity;
		}
		m_UDamageBorder->SetOpacity(fOpacity);
	}
	
	
	m_iCurrentHealth = _iCurrentHealth;
}


void UPlayerHUD::SetComboBarStyle()
{
	m_UComboBarWidget1->WidgetStyle.SetBackgroundImage(ComboBarBackgroundBrush);
	m_UComboBarWidget1->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
	m_UComboBarWidget1->SetPercent(0.f);
	m_UComboBarWidget2->WidgetStyle.SetBackgroundImage(ComboBarBackgroundBrush);
	m_UComboBarWidget2->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
	m_UComboBarWidget2->SetPercent(0.f);
	m_UComboBarWidget3->WidgetStyle.SetBackgroundImage(ComboBarBackgroundBrush);
	m_UComboBarWidget3->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
	m_UComboBarWidget3->SetPercent(0.f);

	m_UL1->SetRenderOpacity(m_fUnenabledButtonsOpacity);
	m_UR1->SetRenderOpacity(m_fUnenabledButtonsOpacity);
}

void UPlayerHUD::SetCurrentComboPercent(float _fCurrentComboPercent)
{
	if (_fCurrentComboPercent >= m_iComboPercentForCut)
	{
		if (!m_bFirstComboBarFilled)
		{
			m_bFirstComboBarFilled = true;
			m_UComboBarWidget1->SetPercent(1.f);
			m_UComboBarWidget1->WidgetStyle.SetFillImage(CutComboBarFillBrush);
			m_UL1->SetRenderOpacity(1.f);
		}

		if (_fCurrentComboPercent >= m_iComboPercentForCut * 2.f)
		{
			if (!m_bSecondComboBarFilled)
			{
				m_bSecondComboBarFilled = true;
				m_UComboBarWidget2->SetPercent(1.f);
				m_UComboBarWidget2->WidgetStyle.SetFillImage(CutComboBarFillBrush);
			}

			if (_fCurrentComboPercent >= m_iComboPercentForCut * 3.f)
			{
				if (!m_bThirdComboBarFilled)
				{
					m_bThirdComboBarFilled = true;
					m_UComboBarWidget3->SetPercent(1.f);
					m_UComboBarWidget1->WidgetStyle.SetFillImage(SpecialComboBarFillBrush);
					m_UComboBarWidget2->WidgetStyle.SetFillImage(SpecialComboBarFillBrush);
					m_UComboBarWidget3->WidgetStyle.SetFillImage(SpecialComboBarFillBrush);
					if (!m_bIsInSpecial)
					{
						m_UR1->SetRenderOpacity(1.f);
					}
				}
			}
			else
			{
				m_UComboBarWidget3->SetPercent((_fCurrentComboPercent - (m_iComboPercentForCut * 2.f)) * 3.f);
				if (m_bThirdComboBarFilled)
				{
					m_bThirdComboBarFilled = false;
					m_UComboBarWidget1->WidgetStyle.SetFillImage(CutComboBarFillBrush);
					m_UComboBarWidget2->WidgetStyle.SetFillImage(CutComboBarFillBrush);
					m_UComboBarWidget3->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
					if (!m_bIsInSpecial)
					{
						m_UR1->SetRenderOpacity(m_fUnenabledButtonsOpacity);
					}
				}
			}
		}
		else
		{
			m_UComboBarWidget2->SetPercent((_fCurrentComboPercent - m_iComboPercentForCut) * 3.f);
			if (m_bSecondComboBarFilled)
			{
				m_bSecondComboBarFilled = false;
				m_UComboBarWidget1->WidgetStyle.SetFillImage(CutComboBarFillBrush);
				m_UComboBarWidget2->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
				m_UComboBarWidget3->SetPercent(0.f);
			}
		}
	}
	else
	{
		m_UComboBarWidget1->SetPercent(_fCurrentComboPercent * 3.f);
		if (m_bFirstComboBarFilled)
		{
			m_bFirstComboBarFilled = false;
			m_UComboBarWidget1->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
			m_UComboBarWidget2->SetPercent(0.f);
			m_UL1->SetRenderOpacity(m_fUnenabledButtonsOpacity);
		}
		if (m_bSecondComboBarFilled)
		{
			m_bSecondComboBarFilled = false;
			m_UComboBarWidget2->SetPercent(0.f);
			m_UComboBarWidget3->SetPercent(0.f);
			m_UComboBarWidget2->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
		}
		if (m_bThirdComboBarFilled)
		{
			m_bThirdComboBarFilled = false;
			m_UComboBarWidget3->SetPercent(0.f);
			m_UComboBarWidget3->WidgetStyle.SetFillImage(InitialComboBarFillBrush);
		}
	}
}

void UPlayerHUD::SetCurrentComboLvl(int _iCurrentComboLvl)
{
	if (_iCurrentComboLvl >= m_iMaxComboLvl) m_UComboLvlText->SetText(FText::FromString("MAX"));
	else m_UComboLvlText->SetText(FText::AsNumber(_iCurrentComboLvl));
}

void UPlayerHUD::SetPercentForCut(float _fComboPercent)
{
	m_iComboPercentForCut = _fComboPercent;
	const float fCurrentComboPercent = m_UComboBarWidget1->Percent;
	/*
	if (fCurrentComboPercent > m_iComboPercentForCut)
	{
		m_UComboBarWidget1->SetFillColorAndOpacity(m_FCutColor);
		m_UComboBarWidget2->SetFillColorAndOpacity(m_FCutColor);
		m_UComboBarWidget3->SetFillColorAndOpacity(m_FCutColor);
	}
	else
	{
		m_UComboBarWidget1->SetFillColorAndOpacity(m_FNoCutColor);
		m_UComboBarWidget2->SetFillColorAndOpacity(m_FNoCutColor);
		m_UComboBarWidget3->SetFillColorAndOpacity(m_FNoCutColor);
	}
	*/
}

void UPlayerHUD::SetMaxComboLvl(int _iMaxComboLvl)
{
	m_iMaxComboLvl = _iMaxComboLvl;
}

void UPlayerHUD::SetComboMaxTime(float _fComboMaxTime)
{
	m_fComboMaxTime = _fComboMaxTime; 
}

void UPlayerHUD::SetComboRemainingTime(float _fComboRemainingTime)
{
	if(m_UTextComboOutlineMaterialInstance)
	{
		if(_fComboRemainingTime <= 0.0f)
		{
			m_UTextComboOutlineMaterialInstance->SetVectorParameterValue("Color", FLinearColor(1.0f,1.0f,1.0f ,0.0f));
		} else
		{
			m_UTextComboOutlineMaterialInstance->SetVectorParameterValue("Color", FLinearColor(1.0f,1.0f,1.0f ,1.0f));
		}
	}
	
	if(m_UTextComboMaterialInstance)
	{
		m_UTextComboMaterialInstance->SetScalarParameterValue("Opacity", _fComboRemainingTime/m_fComboMaxTime);
	}
}

void UPlayerHUD::SetSpecialCurrentDurationNormalized(bool _bActivation, float _fCurrentDuration)
{
	if (_fCurrentDuration > 0.0f)
	{
		if (_bActivation)
		{
			m_bIsInSpecial = true;
		}
		m_iCurrentSpecialDuration = _fCurrentDuration * 2;
		if ((int)m_iCurrentSpecialDuration % 2 == 0)
		{
			m_UR1->SetRenderOpacity(1.f);
		}
		else
		{
			m_UR1->SetRenderOpacity(0.5f);
		}
	}
	else
	{
		m_bIsInSpecial = false;
		m_UR1->SetRenderOpacity(m_fUnenabledButtonsOpacity);
	}
}

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	
	m_UTextComboMaterialInstance = m_UComboLvlText->GetDynamicFontMaterial();
	if(m_UTextComboMaterialInstance)
	{
		m_UTextComboMaterialInstance->SetScalarParameterValue("Opacity", 0);
	}
	m_UTextComboOutlineMaterialInstance = m_UComboLvlText->GetDynamicOutlineMaterial();
	if(m_UTextComboOutlineMaterialInstance)
	{
		m_UTextComboOutlineMaterialInstance->SetVectorParameterValue("Opacity", FLinearColor(1.0f,1.0f,1.0f ,0.0f));
	}
	/*
	m_USpecialProgressBarMaterialInstance = m_USpecialProgressBar->GetDynamicMaterial();
	if(m_USpecialProgressBarMaterialInstance)
	{
		m_USpecialProgressBarMaterialInstance->SetScalarParameterValue("Progress", 0.0f);
	}
	*/

	SetAmmoStyle();
	SetComboBarStyle();
}

void UPlayerHUD::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if(m_UDamageBorder->ColorAndOpacity.A > m_fTargetOpacity)
	{
		const float fNextFrameOpacity = FMath::Clamp(m_UDamageBorder->ColorAndOpacity.A - InDeltaTime * m_fBorderDamageOpacityDecreaseSpeed, m_fTargetOpacity,1.0f);
		m_UDamageBorder->SetOpacity(fNextFrameOpacity);
	}
}
