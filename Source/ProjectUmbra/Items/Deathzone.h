// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Deathzone.generated.h"

class AUmbraGameMode;
class UBoxComponent;
UCLASS()
class PROJECTUMBRA_API ADeathzone : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADeathzone();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Box")
		UBoxComponent* mc_UBox;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Deathzone config", DisplayName = "Gamemode ref")
		AUmbraGameMode* m_pGM;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Deathzone config", DisplayName = "DeathZone Extent")
		FVector m_vDeathzoneExtent;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION()
		void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
