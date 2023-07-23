// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBar.generated.h"

class UProgressBar;

UCLASS()
class PROJECTUMBRA_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(meta = (BindWidget))
		UProgressBar* ProgressBar_70;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Current Health")
		float m_fCurrentHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Max Health")
		float m_fMaxHealth;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attributes", DisplayName = "Is the Player?")
		bool m_bIsThePlayer;

	float GetHealth();
	FColor GetColor();

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

};
