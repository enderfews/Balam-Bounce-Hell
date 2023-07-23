#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"

#include "ProjectUmbraGameInstance.generated.h"

class UFeedbackManager;
class USoundManager;
class UProbabilityManager;

UCLASS()
class PROJECTUMBRA_API UProjectUmbraGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY()
	UProbabilityManager* m_UProbabilityManager = nullptr;
	UPROPERTY()
	USoundManager* m_USoundManager = nullptr;
	UPROPERTY()
	UFeedbackManager* m_UFeedbackManager = nullptr;

	float m_fStoredPlayerHealth = -1.0f;
	float m_fStoredPlayerCombo = -1.0f;
	FString m_fStoredLevelName = "";
	bool m_fStoredPlayerControlScheme = false;

protected:
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FName m_sCurrentStreamingLevelName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool m_bIsMainMenu;

	UFUNCTION(BlueprintCallable)
	UProbabilityManager* GetProbabilityManager();

	UFUNCTION(BlueprintCallable)
	USoundManager* GetSoundManager();

	UFUNCTION(BlueprintCallable)
	UFeedbackManager* GetFeedbackManager();

	virtual void Init() override;
	virtual void Shutdown() override;

	UFUNCTION(BlueprintCallable)
	float GetStoredPlayerHealth() const
	{
		return m_fStoredPlayerHealth;
	}

	UFUNCTION(BlueprintCallable)
	float GetStoredPlayerCombo() const
	{
		return m_fStoredPlayerCombo;
	}

	UFUNCTION(BlueprintCallable)
	bool GetStoredPlayerControlScheme() const
	{
		return m_fStoredPlayerControlScheme;
	}

	UFUNCTION(BlueprintCallable)
	void SetStoredPlayerHealth(float _storedPlayerHealth)
	{
		m_fStoredPlayerHealth = _storedPlayerHealth;
	}

	UFUNCTION(BlueprintCallable)
	void SetStoredPlayerCombo(float _storedPlayerCombo)
	{
		m_fStoredPlayerCombo = _storedPlayerCombo;
	}

	UFUNCTION(BlueprintCallable)
	void SetStoredPlayerControlScheme(bool _storedPlayerControlScheme)
	{
		m_fStoredPlayerControlScheme = _storedPlayerControlScheme;
	}
	
	
};
