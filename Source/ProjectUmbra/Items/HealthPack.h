// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "HealthPack.generated.h"

class APlayerCharacter;
class USphereComponent;

UCLASS()
class PROJECTUMBRA_API AHealthPack : public AActor
{
	GENERATED_BODY()
	
private:
	bool m_bIsMoving = false;
	float m_fCurrentSpeed = 0.0f;
	float m_fCurrentMagnetDuration = 0.0f;
	float m_fCurrentDuration = 0.0f;
	UPROPERTY()
	APlayerCharacter* m_APlayerPtr = nullptr;

	

	UPROPERTY()
	UMaterialInstanceDynamic* m_UMeshMaterial;
	
	UFUNCTION()
	void OnOverlap(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex, bool _bFromSweep, const FHitResult& _FSweepResult);
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* _UOverlappedComponent, AActor* _AOtherActor, UPrimitiveComponent* _UOtherComp, int32 _iOtherBodyIndex);

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Pivot")
	USceneComponent* mc_UPivot;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Trigger")
	USphereComponent* mc_UTrigger;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Components", DisplayName="Mesh")
	UStaticMeshComponent* mc_UMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Heal Amount")
	float m_fHealAmount = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Duration")
	float m_fDuration = 10.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Magnet Radius")
	float m_fMagnetRadius = 300.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Magnet Acceleration")
	float m_fMagnetAcceleration = 100.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Magnet Start Speed")
	float m_fMagnetStartSpeed = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Magnet Max Speed")
	float m_fMagnetMaxSpeed = 1000.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Magnet Max Duration")
	float m_fMagnetMaxDuration = 30.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Config", DisplayName="Distance To Heal")
	float m_fDistanceToHeal = 50.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sounds", DisplayName="Pick Up Sound")
	USoundBase* m_UPickUpSound;
	
	// Sets default values for this actor's properties
	AHealthPack();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
