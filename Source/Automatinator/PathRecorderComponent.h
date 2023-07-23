// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PathRecorderComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AUTOMATINATOR_API UPathRecorderComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	FTimerHandle m_FTimer;
public:	
	// Sets default values for this component's properties
	UPathRecorderComponent();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", DisplayName = "Path Map")
		TMap<FVector, int> m_TPathMap;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Record Rate")
		float m_fRecordRate;
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
	UFUNCTION(BlueprintCallable)
		void SavePaths();
	void RecordPath();
		
};
