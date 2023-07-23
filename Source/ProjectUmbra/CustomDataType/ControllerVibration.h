// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ControllerVibration.generated.h"

class UForceFeedbackEffect;
/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UControllerVibration : public UObject
{
	GENERATED_BODY()
private:
protected:
public:
	UPROPERTY(EditDefaultsOnly, DisplayName="Vibration Name")
	FName m_FVibrationName = NAME_None;
	UPROPERTY(EditDefaultsOnly, DisplayName="Loops")
	bool m_bLoops = false;
	UPROPERTY(EditDefaultsOnly, DisplayName="Ignore Time Dilation")
	bool m_bIgnoreTimeDilation = false;
	UPROPERTY(EditDefaultsOnly, DisplayName="Play While Paused")
	bool m_bPlayWhilePaused = false;
	UPROPERTY(EditDefaultsOnly, DisplayName="Force Feedback Asset", Instanced)
	UForceFeedbackEffect* m_UForceFeedbackAsset;
};
