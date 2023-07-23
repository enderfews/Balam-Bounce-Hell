// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BorderHighlight.generated.h"


class APlayerCharacter;
class UMeshComponent;
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROJECTUMBRA_API UBorderHighlight : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY()
	TArray<UMeshComponent*> m_TMeshesToHighlight;
	UPROPERTY()
	APlayerCharacter* m_APlayerPtr;

	float m_fAngleCos = 0.0f;
	bool m_bIsActive = false;

	bool CheckIfShouldActivate();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& _FPropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category="Config", DisplayName="Select Mesh By Name", BlueprintReadWrite)
	bool m_bSelectMeshByName = false;
	UPROPERTY(EditDefaultsOnly, Category="Config", DisplayName="Mesh Component Name", BlueprintReadWrite, meta=(EditCondition="m_bSelectMeshByName", EditConditionHides))
	FName m_FMeshComponentName;
	UPROPERTY(EditDefaultsOnly, Category="Config", DisplayName="Range to Show Border", BlueprintReadWrite)
	float m_fRangeToShowBorder = 350.f;
	UPROPERTY(EditDefaultsOnly, Category="Config", DisplayName="Range to Show Border", BlueprintReadWrite, meta=(ClampMax=360.0f, ClampMin=0.0f))
	float m_fAngleToShowBorder = 60.f;
	

	bool m_bBorderEnabled = true;
	UFUNCTION(BlueprintCallable)
		void EnableBorderHighLight();
	UFUNCTION(BlueprintCallable)
		void DisableBorderHighLight();
	// Sets default values for this component's properties
	UBorderHighlight();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
