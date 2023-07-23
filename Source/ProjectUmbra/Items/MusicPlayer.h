#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicPlayer.generated.h"

class UFMODAudioComponent;

UCLASS()
class AMusicPlayer : public AActor
{

	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
public:
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Components", DisplayName="FMOD Audio")
	UFMODAudioComponent* mc_UFMODAudioComponent;

	AMusicPlayer();

	void ParametersChanged(const TMap<FName, float>& _TParameters);
	void AggroChanged(float _fAggroValue);
};
