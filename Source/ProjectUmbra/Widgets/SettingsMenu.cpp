// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenu.h"

#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "ProjectUmbra/System/SoundManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"

void USettingsMenu::SaveNewSettings(float _fNewMasterValue, float _fNewMusicValue, float _fNewEffectsValue, float _fNewUIValue)
{
	m_USndManager->SetMasterVolume(_fNewMasterValue);
	m_USndManager->SetMusicVolume(_fNewMusicValue);
	m_USndManager->SetEffectsVolume(_fNewEffectsValue);
	m_USndManager->SetUIVolume(_fNewUIValue);
}

void USettingsMenu::GetSoundManager()
{
	m_USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
}

float USettingsMenu::GetMasterVolumeValue()
{
	return m_USndManager->GetMasterVolume();
}

float USettingsMenu::GetMusicVolumeValue()
{
	return m_USndManager->GetMusicVolume();
}

float USettingsMenu::GetEffectsVolumeValue()
{
	return m_USndManager->GetEffectsVolume();
}

float USettingsMenu::GetUIVolumeValue()
{
	return m_USndManager->GetUIVolume();
}
