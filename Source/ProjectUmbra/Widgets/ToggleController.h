// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ToggleController.generated.h"


class UTextBlock;

UCLASS()
class PROJECTUMBRA_API UToggleController : public UUserWidget
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes", DisplayName = "Use Controller UI")
		bool m_bUseControllerUI=false;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* m_pText;

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void ControllerToggleWarning();
};
