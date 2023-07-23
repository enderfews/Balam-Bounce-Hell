#include "ProjectUmbraGameInstance.h"

#include "FeedbackManager.h"
#include "ProbabilityManager.h"
#include "SoundManager.h"
#include "ProjectUmbra/CustomProjectSettings/ProjectUmbraSettings.h"

UProbabilityManager* UProjectUmbraGameInstance::GetProbabilityManager()
{
	if(IsValid(m_UProbabilityManager))
		return m_UProbabilityManager;

	return nullptr;
}

USoundManager* UProjectUmbraGameInstance::GetSoundManager()
{
	if(IsValid(m_USoundManager))
		return m_USoundManager;

	return nullptr;
}

UFeedbackManager* UProjectUmbraGameInstance::GetFeedbackManager()
{
	if(IsValid(m_UFeedbackManager))
		return m_UFeedbackManager;

	return nullptr;
}

void UProjectUmbraGameInstance::Init()
{
	Super::Init();

	UProjectUmbraSettings* UDefaultSettings = GetMutableDefault<UProjectUmbraSettings>();
	UDefaultSettings->LoadConfig();
	
	m_UProbabilityManager = NewObject<UProbabilityManager>(
		this,
		FName("Probability Manager"));
	
	m_UProbabilityManager->Init(UDefaultSettings, this);
	
	m_USoundManager = NewObject<USoundManager>(
		this,
		FName("Sound Manager"));

	m_USoundManager->Init(UDefaultSettings, this);

	m_UFeedbackManager = NewObject<UFeedbackManager>(
		this,
		FName("Feedback Manager"));

	m_UFeedbackManager->Init(UDefaultSettings, this);
}

void UProjectUmbraGameInstance::Shutdown()
{
	Super::Shutdown();

	UProjectUmbraSettings* UDefaultSettings = GetMutableDefault<UProjectUmbraSettings>();
	UDefaultSettings->SaveConfig();
}
