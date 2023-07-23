// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectUmbra/Interfaces/IBateable.h"
#include "ProjectUmbra/Items/Interactable.h"
#include "ProjectUmbra/Enums/EnemyType.h"
#include "ProjectUmbra/Structs/CheckpointData.h"
#include "Checkpoint.generated.h"

class USoundManager;
class UFMODEvent;
class ASphereComponent;
class UStaticMeshComponent;
class USceneComponent;
class UBoxComponent;
class UMaterial;
class UPaperFlipbookComponent;
class UWidgetComponent;
class UBorderHighlight;
UCLASS()
class PROJECTUMBRA_API ACheckpoint : public AInteractable, public IIBateable
{
	GENERATED_BODY()
private:
	FTimerHandle m_FTimer;
	UPROPERTY()
	USoundManager* m_USndManager;

public:	
	// Sets default values for this actor's properties
	ACheckpoint();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Materials", DisplayName = "Inactive checkpoint material")
		UMaterial* m_pInactiveMat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Materials", DisplayName = "Active checkpoint material")
		UMaterial* m_pActiveMat;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Body")
		UStaticMeshComponent* mc_UBody;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Box")
		UBoxComponent* mc_UBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Player detection box")
		UBoxComponent* mc_UPlayerDetectionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Icon")
		UWidgetComponent* mc_UIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Icon")
		UPaperFlipbookComponent* mc_UFlipBook;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Border highlight component")
		UBorderHighlight* mc_UBorderHighlight;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Respawn point")
		USceneComponent* mc_URespawnPoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Min life to save")
		float m_fMinHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint Config", DisplayName = "Saved life")
		float m_fSavedHealth;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint Config", DisplayName = "Is checked")
		bool m_bIsChecked;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Max distance to respawn in checkpoint")
		float m_fMaxDistanceToRespawnInCheckpoint;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Checkpoint Respawn time")
		float m_fCheckpointRespawnTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Screen fade")
		bool m_bScreenFade;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Activates with bat?")
		bool m_bActivatesWithBat;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Is invisible ?")
		bool m_bIsInvisible;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Checkpoint Config", DisplayName = "Enemies")
		TArray<FCheckpointData> m_TEnemies;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Heal")
		bool m_bHeal;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Checkpoint Config", DisplayName = "Healing Amount")
		float m_fHealingAmount;
	
	//Classes for enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classes", DisplayName = "Normal enemy class")
		TSubclassOf<ABaseEnemy> m_TNormalClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classes", DisplayName = "Kamikaze enemy class")
		TSubclassOf<ABaseEnemy> m_TKamikazeClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classes", DisplayName = "Halo enemy class")
		TSubclassOf<ABaseEnemy> m_THaloClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classes", DisplayName = "Sniper enemy class")
		TSubclassOf<ABaseEnemy> m_TSniperClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classes", DisplayName = "Shotgun enemy class")
		TSubclassOf<ABaseEnemy> m_TShotgunClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enemy Classes", DisplayName = "Burst enemy class")
		TSubclassOf<ABaseEnemy> m_TBurstClass;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Bat FMOD Event"))
	UFMODEvent* m_UBatFMODEvent;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void CheckEnemies();
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void ResetEnemies();
	bool PlayerIsFarFromCheckpoint();
	UClass* GetEnemyClass(EEnemyType _EnemyType);
	virtual void Interact(ACharacter* pCharacter) override;
	UFUNCTION()
		void OnCheckpointOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnPlayerOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//Implementation of IBateable interface
	UFUNCTION(BlueprintCallable)
		virtual void GetHit_Implementation(FVector _vHitDirection, AActor* _ANewOwner, bool _bPlayerOwnership, float _fAutoAimRange = 0.0f, float _AutoAimAngle = 0.0f) override;

	void StopEnemiesLogic();

	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHit);
	UPROPERTY(BlueprintAssignable)
	FOnHit OnHit;
};
