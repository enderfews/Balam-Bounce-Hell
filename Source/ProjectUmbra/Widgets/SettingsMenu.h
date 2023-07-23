// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenu.generated.h"

class USlider;
class UTextBlock;
class USoundManager;

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API USettingsMenu : public UUserWidget
{
	GENERATED_BODY()
	
private:
	USoundManager* m_USndManager;
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* m_UMasterVolumeValue;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* m_UMusicVolumeValue;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* m_UEffectsVolumeValue;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* m_UUIVolumeValue;

	UFUNCTION(BlueprintCallable)
	void SaveNewSettings(float _fNewMasterValue, float _fNewMusicValue, float _fNewEffectsValue, float _fNewUIValue);
	UFUNCTION(BlueprintCallable)
	void GetSoundManager();
	UFUNCTION(BlueprintCallable)
	float GetMasterVolumeValue();
	UFUNCTION(BlueprintCallable)
	float GetMusicVolumeValue();
	UFUNCTION(BlueprintCallable)
	float GetEffectsVolumeValue();
	UFUNCTION(BlueprintCallable)
	float GetUIVolumeValue();
};
