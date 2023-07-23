#include "MusicPlayer.h"

#include "FMODAudioComponent.h"
#include "ProjectUmbra/System/ProjectUmbraGameInstance.h"
#include "ProjectUmbra/System/SoundManager.h"

void AMusicPlayer::BeginPlay()
{
	Super::BeginPlay();

	USoundManager* USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	USndManager->RegisterMusicPlayer(this);
}

void AMusicPlayer::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	USoundManager* USndManager = GetGameInstance<UProjectUmbraGameInstance>()->GetSoundManager();
	USndManager->UnRegisterMusicPlayer(this);
}

AMusicPlayer::AMusicPlayer()
{
	mc_UFMODAudioComponent = CreateDefaultSubobject<UFMODAudioComponent>(TEXT("FMOD Audio"));
	SetRootComponent(mc_UFMODAudioComponent);
}

void AMusicPlayer::ParametersChanged(const TMap<FName, float>& _TParameters)
{
	for(auto It = _TParameters.CreateConstIterator(); It; ++It)
	{
		if(mc_UFMODAudioComponent->ParameterCache.Contains(It.Key()))
			mc_UFMODAudioComponent->SetParameter(It.Key(), It.Value());
	}
}

void AMusicPlayer::AggroChanged(float _fAggroValue)
{
	if(mc_UFMODAudioComponent->ParameterCache.Contains("Aggro"))
		mc_UFMODAudioComponent->SetParameter("Aggro", _fAggroValue);
}

