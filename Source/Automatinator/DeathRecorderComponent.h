// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeathRecorderComponent.generated.h"

class AUmbraGameMode;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AUTOMATINATOR_API UDeathRecorderComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDeathRecorderComponent();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", DisplayName = "Death Map")
		TMap<FVector, int> m_TDeathMap;
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", DisplayName = "Health Component Ref")
	//	AUmbraGameMode* m_pGameMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Streaming map name")
		FString m_sCurrentLevelName;
	bool m_bActivateRecorder;
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION()
		void RecordOnDeath();

	UFUNCTION(BlueprintCallable)
		void SaveDeaths();
};
