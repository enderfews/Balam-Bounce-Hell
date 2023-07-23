// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoBarWidget.generated.h"

class UHorizontalBox;
class UVerticalBox;

class UImage;
/**
 * 
 */
UCLASS()
class PROJECTUMBRA_API UAmmoBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UImage*> m_TAmmoElements;
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* m_VerticalBox;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* m_HorizontalBoxUpper;
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* m_HorizontalBoxLower;
	
	void SetMaxAmmo(int _iMaxAmmo);
	void SetCurrentAmmo(int _iCurrentAmmo);
	void ChangeColor(FLinearColor _FColor);
};
