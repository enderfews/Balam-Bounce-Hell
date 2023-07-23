// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerDeathDetectorComponent.generated.h"


class APlayerCharacter;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTUMBRA_API UPlayerDeathDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UFUNCTION()
	void PlayerDeath();

protected:
	virtual void BeginPlay() override;
	
public:
	UPlayerDeathDetectorComponent();
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category="Config", DisplayName="Observed Player")
	APlayerCharacter* m_TObservedPlayer = nullptr;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE( FOnPlayerDeath );
	UPROPERTY(BlueprintAssignable)
	FOnPlayerDeath OnPlayerDeath;
};
