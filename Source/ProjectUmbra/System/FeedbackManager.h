// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FeedbackManager.generated.h"

class UControllerVibration;
class UProjectUmbraCameraShake;
class UPerlinNoiseCameraShakePattern;
class UCameraShakeBase;
class UProjectUmbraSettings;
class UProjectUmbraGameInstance;
/**
 * 
 */
UCLASS(Blueprintable)
class PROJECTUMBRA_API UFeedbackManager : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
private:
	UPROPERTY()
	UProjectUmbraSettings* m_UProjectSettings;
	UPROPERTY()
	UProjectUmbraGameInstance* m_UProjectGameInstance;

	UPROPERTY()
	TArray<UCameraShakeBase*> m_TActiveCameraShakes;
	TArray<FName> m_TActiveForceFeedbacks;

	float m_fSlowMotionTransitionInTime = 0.0f;
	float m_fCurrentSlowMotionTransitionInTime = 0.0f;
	float m_fSlowMotionTransitionOutTime = 0.0f;
	float m_fCurrentSlowMotionTransitionOutTime = 0.0f;
	float m_fSlowMotionDuration = 0.0f;
	float m_fTargetSlowMotionAmount = 1.0f;
	float m_fCurrentSlowMotionAmount = 1.0f;

	bool m_bVibration = true;
	float m_fScreenShake = 1.0f;

	virtual void Tick(float _fDeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;

public:
	UFeedbackManager();
	
	void Init(UProjectUmbraSettings* _UProjectSettings, UProjectUmbraGameInstance* _UProjectGameInstance);

	UFUNCTION(BlueprintCallable)
	UCameraShakeBase* PlayCameraShake(TSubclassOf<UCameraShakeBase> _UCameraShakeProfileClass, float _fShakeScale);
	UFUNCTION(BlueprintCallable)
	void StopCameraShake(UCameraShakeBase* _UCameraShakeInstance);
	UFUNCTION(BlueprintCallable)
	void StopAllCameraShakes();

	UFUNCTION(BlueprintCallable)
	void PlaySlowMotionEffect(float _fSlowMotionScale, float _fDuration, float _fTransitionInDuration, float _fTransitionOutDuration);
	UFUNCTION(BlueprintCallable)
	void StopSlowMotionEffect(bool m_bImmediate);

	UFUNCTION(BlueprintCallable)
	void PlayVibrationEffect(const UControllerVibration* _UVibrationProfile);
	UFUNCTION(BlueprintCallable)
	void StopVibrationEffect(const UControllerVibration* _UVibrationProfile);
	UFUNCTION(BlueprintCallable)
	void StopAllVibrationEffects();

	void SetVibration(bool _bVibration);
	bool GetVibration();
	void SetScreenShake(float _fScreenShake);
	float GetScreenShake();
};
