// Fill out your copyright notice in the Description page of Project Settings.


#include "FeedbackManager.h"

#include "ProjectUmbraGameInstance.h"

#include "Kismet/GameplayStatics.h"
#include "ProjectUmbra/CustomDataType/ControllerVibration.h"
#include "ProjectUmbra/CustomProjectSettings/ProjectUmbraSettings.h"

void UFeedbackManager::Tick(float _fDeltaTime)
{
	for(int32 i = 0; i< m_TActiveCameraShakes.Num(); ++i)
	{
		if(m_TActiveCameraShakes[i]->IsFinished())
		{
			m_TActiveCameraShakes.RemoveAtSwap(i);
			--i;
		}
	}
	
	if(m_fSlowMotionDuration - m_fSlowMotionTransitionOutTime > 0.0f)
	{
		m_fSlowMotionDuration -= _fDeltaTime;
	}
	else if(m_fTargetSlowMotionAmount < 1.0f)
	{
		m_fSlowMotionDuration = 0.0f; 
		m_fTargetSlowMotionAmount = 1.0f;
	}

	if(m_fCurrentSlowMotionAmount < m_fTargetSlowMotionAmount && m_fCurrentSlowMotionAmount - m_fTargetSlowMotionAmount != FLT_EPSILON)
	{
		if(m_fCurrentSlowMotionTransitionOutTime > 0.0f)
		{
			m_fCurrentSlowMotionAmount += _fDeltaTime / m_fSlowMotionTransitionOutTime;
			m_fCurrentSlowMotionAmount = FMath::Min(m_fTargetSlowMotionAmount,m_fCurrentSlowMotionAmount);
			m_fCurrentSlowMotionTransitionOutTime -= _fDeltaTime;
		} else
		{
			m_fCurrentSlowMotionAmount = m_fTargetSlowMotionAmount;
		}
		UGameplayStatics::SetGlobalTimeDilation(m_UProjectGameInstance->GetWorld(), m_fCurrentSlowMotionAmount);
	}
	else if(m_fCurrentSlowMotionAmount > m_fTargetSlowMotionAmount && m_fCurrentSlowMotionAmount - m_fTargetSlowMotionAmount != FLT_EPSILON)
	{
		if(m_fCurrentSlowMotionTransitionInTime > 0.0f)
		{
			m_fCurrentSlowMotionAmount += _fDeltaTime / m_fSlowMotionTransitionInTime;
			m_fCurrentSlowMotionAmount = FMath::Min(m_fTargetSlowMotionAmount,m_fCurrentSlowMotionAmount);
			m_fCurrentSlowMotionTransitionInTime -= _fDeltaTime;
		} else
		{
			m_fCurrentSlowMotionAmount = m_fTargetSlowMotionAmount;
		}
		UGameplayStatics::SetGlobalTimeDilation(m_UProjectGameInstance->GetWorld(), m_fCurrentSlowMotionAmount);
	}
}

bool UFeedbackManager::IsTickable() const
{
	return true;
}

bool UFeedbackManager::IsTickableInEditor() const
{
	return false;
}

bool UFeedbackManager::IsTickableWhenPaused() const
{
	return false;
}

TStatId UFeedbackManager::GetStatId() const
{
	return TStatId();
}

UFeedbackManager::UFeedbackManager()
{
}

void UFeedbackManager::Init(UProjectUmbraSettings* _UProjectSettings, UProjectUmbraGameInstance* _UProjectGameInstance)
{
	m_UProjectSettings = _UProjectSettings;
	m_UProjectGameInstance = _UProjectGameInstance;

	m_bVibration = m_UProjectSettings->m_bVibration;
	m_fScreenShake = m_UProjectSettings->m_fScreenShake;
}

UCameraShakeBase* UFeedbackManager::PlayCameraShake(TSubclassOf<UCameraShakeBase> _UCameraShakeProfileClass, float _fShakeScale)
{
	const UObject* WorldContext = m_UProjectGameInstance->GetWorld();
	APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(WorldContext, 0);
	return CamManager->StartCameraShake(_UCameraShakeProfileClass, _fShakeScale * m_fScreenShake);
}

void UFeedbackManager::StopCameraShake(UCameraShakeBase* _UCameraShakeInstance)
{
	if(m_TActiveCameraShakes.Contains(_UCameraShakeInstance))
	{
		const UObject* WorldContext = m_UProjectGameInstance->GetWorld();
		APlayerCameraManager* CamManager = UGameplayStatics::GetPlayerCameraManager(WorldContext, 0);
		CamManager->StopCameraShake(_UCameraShakeInstance);
		m_TActiveCameraShakes.RemoveSwap(_UCameraShakeInstance);
	}
}

void UFeedbackManager::StopAllCameraShakes()
{
	for(int32 i = 0; i< m_TActiveCameraShakes.Num(); ++i)
	{
		StopCameraShake(m_TActiveCameraShakes[i]);
		--i;
	}
}

void UFeedbackManager::PlaySlowMotionEffect(float _fSlowMotionScale, float _fDuration, float _fTransitionInDuration, float _fTransitionOutDuration)
{
	m_fTargetSlowMotionAmount = _fSlowMotionScale;
	m_fSlowMotionDuration = _fDuration;
	m_fSlowMotionTransitionInTime = _fTransitionInDuration;
	m_fCurrentSlowMotionTransitionInTime = _fTransitionInDuration;
	m_fSlowMotionTransitionOutTime = _fTransitionOutDuration;
	m_fCurrentSlowMotionTransitionOutTime = _fTransitionOutDuration;
}

void UFeedbackManager::StopSlowMotionEffect(bool m_bImmediate)
{
	if(m_bImmediate)
	{
		m_fTargetSlowMotionAmount = 1.0f;
		m_fSlowMotionDuration = 0.0f;
		m_fSlowMotionTransitionInTime = 0.0f;
		m_fCurrentSlowMotionTransitionInTime = 0.0f;
		m_fSlowMotionTransitionOutTime = 0.0f;
		m_fCurrentSlowMotionTransitionOutTime = 0.0f;
		m_fCurrentSlowMotionAmount = 1.0f;

		UGameplayStatics::SetGlobalTimeDilation(m_UProjectGameInstance->GetWorld(), m_fCurrentSlowMotionAmount);
	}
	else
	{
		m_fSlowMotionDuration = 0.0f;
		m_fSlowMotionTransitionInTime = 0.0f;
		m_fCurrentSlowMotionTransitionInTime = 0.0f;
		m_fTargetSlowMotionAmount = 1.0f;
	}
}

void UFeedbackManager::PlayVibrationEffect(const UControllerVibration* _UVibrationProfile)
{
	if(m_bVibration)
	{
		FForceFeedbackParameters Params;
	
		Params.Tag = _UVibrationProfile->m_FVibrationName;
		Params.bLooping = _UVibrationProfile->m_bLoops;
		Params.bIgnoreTimeDilation = _UVibrationProfile->m_bIgnoreTimeDilation;
		Params.bPlayWhilePaused = _UVibrationProfile->m_bPlayWhilePaused;
	
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientPlayForceFeedback(_UVibrationProfile->m_UForceFeedbackAsset, Params);
	}
}

void UFeedbackManager::StopVibrationEffect(const UControllerVibration* _UVibrationProfile)
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStopForceFeedback(_UVibrationProfile->m_UForceFeedbackAsset, _UVibrationProfile->m_FVibrationName);
}

void UFeedbackManager::StopAllVibrationEffects()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->ClientStopForceFeedback(nullptr, NAME_None);
}

void UFeedbackManager::SetVibration(bool _bVibration)
{
	m_bVibration = _bVibration;
	if(!_bVibration)
	{
		StopAllVibrationEffects();
	}
	m_UProjectSettings->m_bVibration = _bVibration;
}

bool UFeedbackManager::GetVibration()
{
	return m_UProjectSettings->m_bVibration;
}

void UFeedbackManager::SetScreenShake(float _fScreenShake)
{
	m_fScreenShake = _fScreenShake;
	m_UProjectSettings->m_fScreenShake = _fScreenShake;
}

float UFeedbackManager::GetScreenShake()
{
	return m_UProjectSettings->m_fScreenShake;
}


