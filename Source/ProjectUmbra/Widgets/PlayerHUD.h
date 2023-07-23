// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ProjectUmbra/CustomDataType/BulletLUT.h"
#include "PlayerHUD.generated.h"

class UBorder;
// class UHealthBarWidget;
class UImage;
class UProgressBar;
class UTextBlock;
class UButton;
class UCanvasPanel;
class USizeBox;

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
private:
	int m_iMaxComboLvl = 0;
	float m_iMaxAmmo = 0;
	int m_iMaxHealth = 0;
	int m_iCurrentHealth = 0;

	int m_iCurrentSpecialDuration = 0;

	float m_fTargetOpacity = 0.0f;

	float m_fComboMaxTime = 0.0f;

	float m_iComboPercentForCut = 0.0f;
	bool m_bIsInSpecial = false;
	
	UPROPERTY()
	UMaterialInstanceDynamic* m_UTextComboMaterialInstance;
	UPROPERTY()
	UMaterialInstanceDynamic* m_UTextComboOutlineMaterialInstance;
	// UPROPERTY()
	// UMaterialInstanceDynamic* m_USpecialProgressBarMaterialInstance;
protected:
public:
	// UPROPERTY(meta=(BindWidget))
	// UProgressBar* m_UAmmoBarWidget;
	UPROPERTY(meta = (BindWidget))
	USizeBox* m_UAmmoColor;
	UImage* m_UAmmoImage;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* m_UAmmoText;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* m_UAmmoCanvas;
	// UPROPERTY(meta=(BindWidget))
	// UHealthBarWidget* m_UHealthBarWidget;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* m_UComboBarWidget1;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* m_UComboBarWidget2;
	UPROPERTY(meta=(BindWidget))
	UProgressBar* m_UComboBarWidget3;
	// UPROPERTY(meta=(BindWidget))
	// UBorder* m_USpecialProgressBar;
	UPROPERTY(meta = (BindWidget))
	UButton* m_UL1;
	UPROPERTY(meta = (BindWidget))
	UButton* m_UR1;
	UPROPERTY(meta=(BindWidget))
	UTextBlock* m_UComboLvlText;
	UPROPERTY(meta=(BindWidget))
	UImage* m_UDamageBorder;
	UPROPERTY(meta = (BindWidget))
	UCanvasPanel* m_UHealthCanvas;

	UPROPERTY(EditAnywhere)
	float m_fBorderDamageOpacityDecreaseSpeed = 1.0f;
	UPROPERTY(EditAnywhere)
	float m_fOnHitOpacity = 0.3f;
	UPROPERTY(EditAnywhere)
	int32 m_iHealthToMaintainBorder = 2;

	UPROPERTY(EditAnywhere)
	FLinearColor m_FCutColor = FLinearColor(FVector(1.0,0.723055,0.116971));
	UPROPERTY(EditAnywhere)
	FLinearColor m_FNoCutColor = FLinearColor(FVector(0.223958,0.161934,0.026197));

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Unenabled Buttons Opacity"))
	float m_fUnenabledButtonsOpacity = 0.f;

	UPROPERTY(EditAnywhere, meta = (DisplayName = "Combo Bar Background Brush"))
	FSlateBrush ComboBarBackgroundBrush;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Initial Combo Bar Fill Brush"))
	FSlateBrush InitialComboBarFillBrush;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Cut Combo Bar Fill Brush"))
	FSlateBrush CutComboBarFillBrush;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Special Combo Bar Fill Brush"))
	FSlateBrush SpecialComboBarFillBrush;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "No Ammo Brush"))
	FSlateBrush NoAmmoBrush;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Explosive Ammo Brush"))
	FSlateBrush ExplosiveAmmoBrush;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Freezing Ammo Brush"))
	FSlateBrush FreezingAmmoBrush;
	UPROPERTY(EditAnywhere, meta = (DisplayName = "Bouncy Ammo Brush"))
	FSlateBrush BouncyAmmoBrush;
	
	void SetAmmoStyle();
	void SetMaxAmmo(int _iMaxAmmo);
	void SetCurrentAmmo(int _iCurrentAmmo);
	void SetPercentForCut(float _fComboPercent);
	void ChangeAmmoTag(EBulletModifier _ENewTag);

	void SetMaxHealth(int _iMaxHealth);
	void SetCurrentHealth(int _iCurrentHealth);

	void SetComboBarStyle();
	void SetCurrentComboPercent(float _fCurrentComboPercent);
	void SetCurrentComboLvl(int _iCurrentComboLvl);
	void SetMaxComboLvl(int _iMaxComboLvl);
	void SetComboMaxTime(float _fComboMaxTime);
	void SetComboRemainingTime(float _fComboRemainingTime);

	void SetSpecialCurrentDurationNormalized(bool _bActivation, float _fCurrentDuration);

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	bool m_bFirstComboBarFilled = false;
	bool m_bSecondComboBarFilled = false;
	bool m_bThirdComboBarFilled = false;
};


