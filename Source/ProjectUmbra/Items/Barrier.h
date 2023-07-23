// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Barrier.generated.h"

class UTextRenderComponent;
class ABaseEnemy;

UCLASS()
class PROJECTUMBRA_API ABarrier : public AActor
{
	GENERATED_BODY()
	
private:
	bool m_bIsOpening = false;
	bool m_bIsClosing = false;
	float m_fMovementProgress = 0;

	FVector m_VStartPos;
	FVector m_VEndPos;

	int m_iCounter = 0;

	UFUNCTION()
	void OnEnemyDeath();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Pivot")
	USceneComponent* mc_UPivot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Wall")
	UStaticMeshComponent* mc_UWall;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Counter")
	UTextRenderComponent* mc_UCounter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Enemies")
	TArray<ABaseEnemy*> TEnemies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Start Color")
	FColor m_FStartColor = FColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="End Color")
	FColor m_FEndColor = FColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Start Pos")
	FVector m_VLocalStartPos = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="End Pos")
	FVector m_VLocalEndPos = FVector::ZeroVector;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Configuration", DisplayName="Close Speed")
	float m_fCloseSpeed = 1.f;
	
	// Sets default values for this actor's properties
	ABarrier();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
#if WITH_EDITOR
	virtual bool ShouldTickIfViewportsOnly() const override;
	virtual void EditorApplyScale(const FVector& DeltaScale, const FVector* PivotLocation, bool bAltDown, bool bShiftDown, bool bCtrlDown) override;
#endif

	UFUNCTION(BlueprintCallable)
	void Open();
	UFUNCTION(BlueprintCallable)
	void Close();
};
