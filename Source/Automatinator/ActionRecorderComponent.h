// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ActionRecorderComponent.generated.h"


UENUM(BlueprintType)
enum class EPlayerActionType : uint8
{
	Cut = 0 UMETA(DisplayName = "Cut"),
	Special = 1 UMETA(DisplayName = "Special"),
	Bat = 1 << 1 UMETA(DisplayName = "Bat"),
	Dash = 1 << 2 UMETA(DisplayName = "Dash")
};
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class AUTOMATINATOR_API UActionRecorderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UActionRecorderComponent();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", DisplayName = "Cut Map")
		TMap<FVector, int> m_TCutMap;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", DisplayName = "Special Map")
		TMap<FVector, int> m_TSpecialMap;
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
		void Record(EPlayerActionType eType);
	UFUNCTION()
		void RecordOnCut();
	UFUNCTION()
		void RecordOnSpecial();
	UFUNCTION(BlueprintCallable)
	void SaveCutRecords();
	UFUNCTION(BlueprintCallable)
	void SaveSpecialRecords();
};
