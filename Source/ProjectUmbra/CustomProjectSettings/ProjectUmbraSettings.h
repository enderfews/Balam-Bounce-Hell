// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ProjectUmbraSettings.generated.h"

/**
 * 
 */
UCLASS(config = ProjectUmbraSettings)
class PROJECTUMBRA_API UProjectUmbraSettings : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, config, Category = "Sound|Volumes", DisplayName="Master Volume", meta=(ClampMin=0.0001f, ClampMax=1.0f))
	float m_fMasterVolume = 1.0f;
	UPROPERTY(EditAnywhere, config, Category = "Sound|Volumes", DisplayName="Music Volume", meta=(ClampMin=0.0001f, ClampMax=1.0f))
	float m_fMusicVolume = 1.0f;
	UPROPERTY(EditAnywhere, config, Category = "Sound|Volumes", DisplayName="Effects Volume", meta=(ClampMin=0.0001f, ClampMax=1.0f))
	float m_fEffectsVolume = 1.0f;
	UPROPERTY(EditAnywhere, config, Category = "Sound|Volumes", DisplayName="UI Volume", meta=(ClampMin=0.0001f, ClampMax=1.0f))
	float m_fUIVolume = 1.0f;

	/** Time in seconds the aggro parameter will take to change one unit */
	UPROPERTY(EditAnywhere, config, Category = "Sound|Aggro Configuration", DisplayName="Aggro Increase Change Time")
	float m_fAggroIncreaseChangeTime = 1.0f;
	/** Time in seconds the aggro parameter will take to change one unit */
    UPROPERTY(EditAnywhere, config, Category = "Sound|Aggro Configuration", DisplayName="Aggro Decrease Change Time")
    float m_fAggroDecreaseChangeTime = 1.0f;
	/** When no enemies are in combat, how much time until the aggro starts going down */
	UPROPERTY(EditAnywhere, config, Category = "Sound|Aggro Configuration", DisplayName="Aggro Maintain Time")
	float m_fAggroMaintainTime = 5.0f;
	/** Number of enemies targeting the player needed for the aggro to be 1 */
	UPROPERTY(EditAnywhere, config, Category = "Sound|Aggro Configuration", DisplayName="Enemies To Activate Aggro Low")
	int m_iEnemiesToActivateAggroLow = 1;
	/** Number of enemies targeting the player needed for the aggro to be 2 */
	UPROPERTY(EditAnywhere, config, Category = "Sound|Aggro Configuration", DisplayName="Enemies To Activate Aggro Hard")
	int m_iEnemiesToActivateAggroHard = 4;

	
	UPROPERTY(EditAnywhere, config, Category = "Probability|Health Drops", DisplayName="Probability At 1 Health", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fProbabilityAt1Health = 1.0f;
	UPROPERTY(EditAnywhere, config, Category = "Probability|Health Drops", DisplayName="Probability At 2 Health", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fProbabilityAt2Health = 0.66f;
	UPROPERTY(EditAnywhere, config, Category = "Probability|Health Drops", DisplayName="Probability At 3 Health", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fProbabilityAt3Health = 0.33f;
	UPROPERTY(EditAnywhere, config, Category = "Probability|Health Drops", DisplayName="Probability At 4 Health", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fProbabilityAt4Health = 0.15f;
	UPROPERTY(EditAnywhere, config, Category = "Probability|Health Drops", DisplayName="Probability At 5 Health", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fProbabilityAt5Health = 0.0f;

	UPROPERTY(EditAnywhere, config, Category = "Probability|Tag Drops", DisplayName="Base Probability", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fBaseProbability = 1.0f;
	UPROPERTY(EditAnywhere, config, Category = "Probability|Tag Drops", DisplayName="Probability Reduction Per Floor Drop", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fProbabilityReductionPerFloorDrop = 0.5f;
	UPROPERTY(EditAnywhere, config, Category = "Probability|Tag Drops", DisplayName="Probability Reduction If Player Has Tag", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fProbabilityReductionIfPlayerHasTag = 0.5f;

	UPROPERTY(EditAnywhere, config, Category = "Feedback", DisplayName="Vibration")
	bool m_bVibration = true;
	UPROPERTY(EditAnywhere, config, Category = "Feedback", DisplayName="Screen Shake", meta=(ClampMin=0.0f, ClampMax=1.0f))
	float m_fScreenShake = 1.0f;
};
