// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SettingsMenuGraphics.generated.h"

class UGameUserSettings;
class UCheckBox;
class UFeedbackManager;

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API USettingsMenuGraphics : public UUserWidget
{
	GENERATED_BODY()
	
private:
	UGameUserSettings* m_UGameUserSettings;
	bool m_bIsInQualityMode = false;
	FIntPoint m_ipQualityModeResolution;
	UFeedbackManager* m_UFeedbackManager;
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Resolution"))
	FIntPoint m_ipResolution;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Render Scale % (50..100)"))
	float m_fRenderScale = 50.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "View Distance (0..4)"))
	int32 m_iViewDistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Anti-Aliasing (0..4)"))
	int32 m_iAntiAliasing = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Post Processing Quality (0..4)"))
	int32 m_iPostProcessingQuality = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Shadows Quality (0..4)"))
	int32 m_iShadowsQuality = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Texture Quality (0..4)"))
	int32 m_iTextureQuality = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Effects Quality (0..4)"))
	int32 m_iEffectsQuality = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Foliage Quality (0..4)"))
	int32 m_iFoliageQuality = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Performance Graphics", meta = (DisplayName = "Shading Quality (0..4)"))
	int32 m_iShadingQuality = 0;

	UFUNCTION(BlueprintCallable)
	void GetGameUserSettings();
	UFUNCTION(BlueprintCallable)
	bool GetVSync();
	UFUNCTION(BlueprintCallable)
	FString GetCurrentMode();
	UFUNCTION(BlueprintCallable)
	bool GetGamepadVibration();
	UFUNCTION(BlueprintCallable)
	float GetScreenShake();
	UFUNCTION(BlueprintCallable)
	void SaveNewGraphicSettings(bool _bVSyncEnabled, bool _bPerformanceMode, bool _bGamepadVibration, float _fNewScreenShakeValue);
};
