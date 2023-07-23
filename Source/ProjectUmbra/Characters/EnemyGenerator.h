// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectUmbra/Characters/BaseEnemy.h"
#include "ProjectUmbra/Enums/EnemyType.h"
#include "ProjectUmbra/Structs/WaypointData.h"
#include "EnemyGenerator.generated.h"

class UNiagaraSystem;
class UFMODAudioComponent;
class USoundManager;
class UFMODEvent;
class UStaticMeshComponent;
class UHealthComponentCpp;
class USphereComponent;
class UWidgetComponent;
class ABaseEnemy;

UCLASS()
class PROJECTUMBRA_API AEnemyGenerator : public ABaseEnemy
{
	GENERATED_BODY()
private:
	bool m_bInitialWaveRespawned;
	bool m_bPlayerDetected;
	bool m_bIsExploding = false;

	UPROPERTY()
	USoundManager* m_USndManager;
	UPROPERTY()
	UFMODAudioComponent* m_UIdleAudioComponent;

	FTimerHandle m_FExplosionTimerHandle;
	FTimerHandle m_FFirstSpawnTimerHandle;

	void DestroyEnemies();
	void StunEnemies();
	void ClearRefs();
	void Explode();
	void PlaySpawnSound();
	
public:
	AEnemyGenerator();

	//Attributes
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Config", DisplayName = "Generator live")
		float m_fGeneratorLive;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Config", DisplayName = "Detection radius")
		float m_fDetectionRadius;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Config", DisplayName = "Activate entering area")
		bool m_bActivateEnteringArea;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Test config", DisplayName = "Is Test Mode?")
		bool m_bTestMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Config", DisplayName = "Waypoint data")
		TArray<FWaypointData> m_TWaypointData;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Config", DisplayName = "Delay Before Explosion")
	float m_fDelayBeforeExplosion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Generator Config", DisplayName = "Time Offset for Spawn Sound")
	float m_fTimeOffsetForSpawnSound = 1.5f;

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

	//Components
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Sphere")
		UStaticMeshComponent* mc_USphere;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Cone")
		UStaticMeshComponent* mc_UCone;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "BodyMesh")
		UStaticMeshComponent* mc_UBodyMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Detection area")
		USphereComponent* mc_UDetectionArea;

	//Sounds
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Idle FMOD Event"))
	UFMODEvent* m_UIdleFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Charge FMOD Event"))
	UFMODEvent* m_UChargeFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Explode FMOD Event"))
	UFMODEvent* m_UExplodeFMODEvent;
	UPROPERTY(EditAnywhere, Category="Sounds", meta=(DisplayName="Spawn Enemy FMOD Event"))
	UFMODEvent* m_USpawnEnemyFMODEvent;

	//Particles
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Destroy Particle System"))
	UNiagaraSystem* m_UDestroyParticleSystem;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Particles", meta = (DisplayName = "Spawn Enemy Particle System"))
	UNiagaraSystem* m_USpawnEnemyParticleSystem;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform& Transform) override;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		void CheckEnemyNumbers(int index);
	UFUNCTION(BlueprintCallable)
		void RespawnInitialWave();

	UClass* GetEnemyClass(EEnemyType _EnemyType);
	void SetRandomStadistics(ABaseEnemy* _Enemy, EEnemyType _EnemyType, int _WaypointDataIndex);
	void CheckValidEnemyRefs(TArray<ABaseEnemy*>& _Refs, int _WaypointIndex, int _ArrayIndex);
	void RespawnMoreEnemies(AWaypoint* _Waypoint, EEnemyType _Type, int _AmountToSpawn, TArray<ABaseEnemy*>& _ArrayRefs, int _WaypointIndex, int _ArrayIndex);
	UFUNCTION()
		void OnEnteringArea(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnGeneratorDestroyed(AActor* DestroyedActor);

	UFUNCTION()
	void OnSpawnedEnemyDeath();

	UFUNCTION(BlueprintCallable)
	void OnPlayerDeath();

	virtual void OnDeathCallback() override;
};
