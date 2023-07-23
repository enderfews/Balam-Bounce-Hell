#include "SoundManager.h"

#include "FMODBlueprintStatics.h"
#include "FMODEvent.h"
#include "ProjectUmbraGameInstance.h"
#include "ProjectUmbra/CustomProjectSettings/ProjectUmbraSettings.h"
#include "ProjectUmbra/Items/MusicPlayer.h"

USoundManager::USoundManager()
{
	if (IFMODStudioModule::IsAvailable())
	{
		FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
		if (StudioSystem)
		{
			StudioSystem->getBus(TCHAR_TO_UTF8(*FName("bus:/Master").ToString()), &MasterBus);
			StudioSystem->getBus(TCHAR_TO_UTF8(*FName("bus:/Master/Music").ToString()), &MusicBus);
			StudioSystem->getBus(TCHAR_TO_UTF8(*FName("bus:/Master/Effects").ToString()), &EffectsBus);
			StudioSystem->getBus(TCHAR_TO_UTF8(*FName("bus:/Master/UI").ToString()), &UIBus);
		}
	}
}

void USoundManager::Init(UProjectUmbraSettings* _UProjectSettings, UProjectUmbraGameInstance* _UProjectGameInstance)
{
	m_UProjectSettings = _UProjectSettings;
	m_UProjectGameInstance = _UProjectGameInstance;

	m_fMasterVolume = _UProjectSettings->m_fMasterVolume;
	m_fMusicVolume = _UProjectSettings->m_fMusicVolume;
	m_fEffectsVolume = _UProjectSettings->m_fEffectsVolume;
	m_fUIVolume = _UProjectSettings->m_fUIVolume;

	MasterBus->setVolume(m_fMasterVolume);
	MusicBus->setVolume(m_fMusicVolume);
	EffectsBus->setVolume(m_fEffectsVolume);
	UIBus->setVolume(m_fUIVolume);

	m_fAggroIncreaseChangeTime = _UProjectSettings->m_fAggroIncreaseChangeTime;
	m_fAggroDecreaseChangeTime = _UProjectSettings->m_fAggroDecreaseChangeTime;
	m_iEnemiesToActivateAggroLow = _UProjectSettings->m_iEnemiesToActivateAggroLow;
	m_iEnemiesToActivateAggroHard = _UProjectSettings->m_iEnemiesToActivateAggroHard;
}

void USoundManager::Tick(float _fDeltaTime)
{
	if(m_fAggro < m_fTargetAggro && m_fAggro - m_fTargetAggro != FLT_EPSILON)
	{
		m_fAggro += _fDeltaTime / m_fAggroIncreaseChangeTime;
		m_fAggro = FMath::Min(m_fTargetAggro,m_fAggro);
	}
	else if(m_fAggro > m_fTargetAggro && m_fAggro - m_fTargetAggro != FLT_EPSILON)
	{
		if(m_fCurrentAggroMaintainTime > 0.0f)
		{
			m_fCurrentAggroMaintainTime -= _fDeltaTime;
			return;
		}
		m_fAggro -= _fDeltaTime / m_fAggroDecreaseChangeTime;
		m_fAggro = FMath::Max(m_fTargetAggro,m_fAggro);
	}

	for(uint8 i = 0; i < m_TMusicPlayers.Num(); ++i)
	{
		if(IsValid(m_TMusicPlayers[i]))
		{
			m_TMusicPlayers[i]->AggroChanged(m_fAggro);
		}
		else
		{
			m_TMusicPlayers.RemoveAt(i);
			--i;
		}
	}
}

bool USoundManager::IsTickable() const
{
	return true;
}

bool USoundManager::IsTickableInEditor() const
{
	return false;
}

bool USoundManager::IsTickableWhenPaused() const
{
	return false;
}

TStatId USoundManager::GetStatId() const
{
	return TStatId();
}

void USoundManager::AddAggro()
{
	m_iCurrentAggroedEnemies++;
	CalculateNewAggro();
}

void USoundManager::RemoveAggro()
{
	m_iCurrentAggroedEnemies--;
	CalculateNewAggro();
}

void USoundManager::CalculateNewAggro()
{
	if(m_iCurrentAggroedEnemies >= m_iEnemiesToActivateAggroHard)
	{
		m_fCurrentAggroMaintainTime = 0.0f;
		m_fTargetAggro = 2.0f;
	}
	else if(m_iCurrentAggroedEnemies >= m_iEnemiesToActivateAggroLow)
	{
		m_fCurrentAggroMaintainTime = 0.0f;
		m_fTargetAggro = 1.0f;
	}
	else if(m_fTargetAggro > 0.0f)
	{
		m_fCurrentAggroMaintainTime = m_fAggroMaintainTime;
		m_fTargetAggro = 0.0f;
	}
}

void USoundManager::ModifyParameter(FName _FParameterName, float _fParameterValue)
{
	if(m_TFMODParams.Contains(_FParameterName))
	{
		m_TFMODParams[_FParameterName] = _fParameterValue;
	}
	else
	{
		m_TFMODParams.Add(_FParameterName, _fParameterValue);
	}

	for(uint8 i = 0; i < m_TMusicPlayers.Num(); ++i)
	{
		if(IsValid(m_TMusicPlayers[i]))
		{
			m_TMusicPlayers[i]->ParametersChanged(m_TFMODParams);
		}
		else
		{
			m_TMusicPlayers.RemoveAt(i);
			--i;
		}
	}
}

void USoundManager::RegisterMusicPlayer(AMusicPlayer* _AMusicPlayerToRegister)
{
	m_TMusicPlayers.Add(_AMusicPlayerToRegister);
}

void USoundManager::UnRegisterMusicPlayer(AMusicPlayer* _AMusicPlayerToUnRegister)
{
	m_TMusicPlayers.Remove(_AMusicPlayerToUnRegister);
}

FFMODEventInstance USoundManager::PlayEventAtLocation(UObject* _UWorldContext, UFMODEvent* _UFMODEvent, const FVector& _VLocation, bool _bAutoPlay)
{
	return UFMODBlueprintStatics::PlayEventAtLocation(_UWorldContext, _UFMODEvent, FTransform(_VLocation), _bAutoPlay);
}

UFMODAudioComponent* USoundManager::PlayEventAttached(UFMODEvent* _UFMODEvent, USceneComponent* _USceneComponentToAttach, const FVector& _VLocation,
	EAttachLocation::Type _EAttachLocationType, bool _bStopWhenAttachedDestroyed, bool _bAutoPlay, bool _bAutoDestroy)
{
	return UFMODBlueprintStatics::PlayEventAttached(_UFMODEvent, _USceneComponentToAttach, "", _VLocation,
		_EAttachLocationType, _bStopWhenAttachedDestroyed, _bAutoPlay, _bAutoDestroy);
}

FFMODEventInstance USoundManager::PlayEventUI(UObject* _UWorldContext, UFMODEvent* _UFMODEvent, bool _bAutoPlay)
{
	return UFMODBlueprintStatics::PlayEvent2D(_UWorldContext, _UFMODEvent, _bAutoPlay);
}

float USoundManager::GetMasterVolume()
{
	return m_fMasterVolume;
}

float USoundManager::GetMusicVolume()
{
	return m_fMusicVolume;
}

float USoundManager::GetEffectsVolume()
{
	return m_fEffectsVolume;
}

float USoundManager::GetUIVolume()
{
	return m_fUIVolume;
}

void USoundManager::SetMasterVolume(float _fMasterVolume)
{
	m_fMasterVolume = FMath::Clamp(_fMasterVolume, 0.0001f, 1.0f);
	
	MasterBus->setVolume(m_fMasterVolume);

	m_UProjectSettings->m_fMasterVolume = m_fMasterVolume;
}

void USoundManager::SetMusicVolume(float _fMusicVolume)
{
	m_fMusicVolume = FMath::Clamp(_fMusicVolume, 0.0001f, 1.0f);
	
	MusicBus->setVolume(m_fMusicVolume);

	m_UProjectSettings->m_fMusicVolume = m_fMusicVolume;
}

void USoundManager::SetEffectsVolume(float _fEffectsVolume)
{
	m_fEffectsVolume = FMath::Clamp(_fEffectsVolume, 0.0001f, 1.0f);

	EffectsBus->setVolume(_fEffectsVolume);

	m_UProjectSettings->m_fEffectsVolume = m_fEffectsVolume;
}

void USoundManager::SetUIVolume(float _fUIVolume)
{
	m_fUIVolume = FMath::Clamp(_fUIVolume, 0.0001f, 1.0f);
	
	UIBus->setVolume(m_fUIVolume);
	
	m_UProjectSettings->m_fUIVolume = m_fUIVolume;
}
