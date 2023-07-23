#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "Engine/EngineTypes.h"
#include "UObject/Object.h"
#include "SoundManager.generated.h"

class UProjectUmbraGameInstance;
class UProjectUmbraSettings;
class AMusicPlayer;
class UFMODAudioComponent;
class UFMODEvent;

UCLASS(Blueprintable)
class PROJECTUMBRA_API USoundManager : public UObject, public FTickableGameObject
{
	GENERATED_BODY()
private:
	float m_fMasterVolume = 1.0f;
	float m_fMusicVolume = 1.0f;
	float m_fEffectsVolume = 1.0f;
	float m_fUIVolume = 1.0f;

	FMOD::Studio::Bus* MasterBus;
	FMOD::Studio::Bus* MusicBus;
	FMOD::Studio::Bus* EffectsBus;
	FMOD::Studio::Bus* UIBus;

	TMap<FName, float> m_TFMODParams;
	UPROPERTY()
	TArray<AMusicPlayer*> m_TMusicPlayers;
	float m_fAggro = 0.0f;
	float m_fTargetAggro = 0.0f;
	float m_fAggroIncreaseChangeTime = 1.0f;
	float m_fAggroDecreaseChangeTime = 1.0f;
	float m_fAggroMaintainTime = 5.0f;
	float m_fCurrentAggroMaintainTime = 0.0f;
	int m_iEnemiesToActivateAggroLow = 1;
	int m_iEnemiesToActivateAggroHard = 4;
	int m_iCurrentAggroedEnemies = 0;

	UPROPERTY()
	UProjectUmbraSettings* m_UProjectSettings;
	UPROPERTY()
	UProjectUmbraGameInstance* m_UProjectGameInstance;
	
	virtual void Tick(float _fDeltaTime) override;
	virtual bool IsTickable() const override;
	virtual bool IsTickableInEditor() const override;
	virtual bool IsTickableWhenPaused() const override;
	virtual TStatId GetStatId() const override;
	void CalculateNewAggro();
	
public:
	USoundManager();
	
	void Init(UProjectUmbraSettings* _UProjectSettings, UProjectUmbraGameInstance* _UProjectGameInstance);

	void AddAggro();
	void RemoveAggro();

	UFUNCTION(BlueprintCallable)
	void ModifyParameter(FName _FParameterName, float _fParameterValue);

	void RegisterMusicPlayer(AMusicPlayer* _AMusicPlayerToRegister);
	void UnRegisterMusicPlayer(AMusicPlayer* _AMusicPlayerToUnRegister);

	FFMODEventInstance PlayEventAtLocation(UObject* _UWorldContext, UFMODEvent* _UFMODEvent, const FVector& _VLocation, bool _bAutoPlay = true);
	UFMODAudioComponent* PlayEventAttached(UFMODEvent* _UFMODEvent, USceneComponent* _USceneComponentToAttach, const FVector& _VLocation, EAttachLocation::Type _EAttachLocationType = EAttachLocation::KeepRelativeOffset, bool _bStopWhenAttachedDestroyed = true, bool _bAutoPlay = true, bool _bAutoDestroy = true);
	FFMODEventInstance PlayEventUI(UObject* _UWorldContext, UFMODEvent* _UFMODEvent, bool _bAutoPlay = true);

	float GetMasterVolume();
	float GetMusicVolume();
	float GetEffectsVolume();
	float GetUIVolume();

	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float _fMasterVolume);
	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float _fMusicVolume);
	UFUNCTION(BlueprintCallable)
	void SetEffectsVolume(float _fEffectsVolume);
	UFUNCTION(BlueprintCallable)
	void SetUIVolume(float _fUIVolume);
};
