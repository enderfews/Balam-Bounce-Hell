// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HealthBarWidget.generated.h"

class UImage;
class UHorizontalBox;

/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UHealthBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UImage*> m_UHearths;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* m_HorizontalBox;

	void SetMaxHealth(int _iMaxHealth);
	void SetCurrentHealth(int _iCurrentHealth);
};
