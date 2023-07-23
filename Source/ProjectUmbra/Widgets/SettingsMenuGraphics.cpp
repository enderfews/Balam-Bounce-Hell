// Fill out your copyright notice in the Description page of Project Settings.


#include "SettingsMenuGraphics.h"
#include "GameFramework/GameUserSettings.h"
#include "Components/CheckBox.h"
#include "ProjectUmbra/System/FeedbackManager.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"

void USettingsMenuGraphics::GetGameUserSettings()
{
	m_UGameUserSettings = UGameUserSettings::GetGameUserSettings();
	m_ipQualityModeResolution.X = 1920;
	m_ipQualityModeResolution.Y = 1080;
	m_UFeedbackManager = GetGameInstance<UProjectUmbraGameInstance>()->GetFeedbackManager();
	m_UGameUserSettings->SetDynamicResolutionEnabled(true);
	m_UGameUserSettings->ApplySettings(false);
}

bool USettingsMenuGraphics::GetVSync()
{
	return m_UGameUserSettings->IsVSyncEnabled();
}

FString USettingsMenuGraphics::GetCurrentMode()
{
	if (m_UGameUserSettings->GetShadowQuality() < 3)
	{
		return "Performance Mode";
	}
	else
	{
		return "Quality Mode";
	}
}

bool USettingsMenuGraphics::GetGamepadVibration()
{
	return m_UFeedbackManager->GetVibration();
}

float USettingsMenuGraphics::GetScreenShake()
{
	return m_UFeedbackManager->GetScreenShake();
}

void USettingsMenuGraphics::SaveNewGraphicSettings(bool _bVSyncEnabled, bool _bPerformanceMode, bool _bGamepadVibration, float _fNewScreenShakeValue)
{
	m_UGameUserSettings->SetVSyncEnabled(_bVSyncEnabled);

	if (_bPerformanceMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Performance Mode"));
		m_UGameUserSettings->SetScreenResolution(m_ipResolution);
		m_UGameUserSettings->SetResolutionScaleValueEx(m_fRenderScale);
		m_UGameUserSettings->SetViewDistanceQuality(m_iViewDistance);
		m_UGameUserSettings->SetAntiAliasingQuality(m_iAntiAliasing);
		m_UGameUserSettings->SetPostProcessingQuality(m_iPostProcessingQuality);
		m_UGameUserSettings->SetShadowQuality(m_iShadowsQuality);
		m_UGameUserSettings->SetTextureQuality(m_iTextureQuality);
		m_UGameUserSettings->SetVisualEffectQuality(m_iEffectsQuality);
		m_UGameUserSettings->SetFoliageQuality(m_iFoliageQuality);
		m_UGameUserSettings->SetShadingQuality(m_iShadingQuality);

		m_bIsInQualityMode = false;
	}
	else if (!_bPerformanceMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("Quality Mode"));
		m_UGameUserSettings->SetScreenResolution(m_ipQualityModeResolution);
		m_UGameUserSettings->SetResolutionScaleValueEx(100.f);
		m_UGameUserSettings->SetViewDistanceQuality(3);
		m_UGameUserSettings->SetAntiAliasingQuality(3);
		m_UGameUserSettings->SetPostProcessingQuality(3);
		m_UGameUserSettings->SetShadowQuality(3);
		m_UGameUserSettings->SetTextureQuality(3);
		m_UGameUserSettings->SetVisualEffectQuality(3);
		m_UGameUserSettings->SetFoliageQuality(3);
		m_UGameUserSettings->SetShadingQuality(3);

		m_bIsInQualityMode = true;
	}
	m_UGameUserSettings->ApplySettings(false);

	m_UFeedbackManager->SetVibration(_bGamepadVibration);
	m_UFeedbackManager->SetScreenShake(_fNewScreenShakeValue);
}