// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectUmbra/Enums/EnemyType.h"
#include "ProjectUmbra/Structs/WaypointData.h"
#include "PuntualEnemyRespawn.generated.h"

class UEnemyDeathDetectorComponent;
class ABaseEnemy;
UCLASS()
class PROJECTUMBRA_API APuntualEnemyRespawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APuntualEnemyRespawn();
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components", DisplayName = "Death detector")
		UEnemyDeathDetectorComponent* mc_UDeathDetector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn config", DisplayName = "Waypoint data")
		TArray<FWaypointData> m_TWaypoints;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Respawn config", DisplayName = "Number of waves")
		int m_iNumOfWaves;

	int m_iAmmountOfEnemies;
	int m_iActualWave;

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
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	UFUNCTION()
		void OnAllEnemiesDeathRespawn();
	UFUNCTION()
		void OnEnemyDeath();

	UClass* GetEnemyClass(EEnemyType _EnemyType);
	void SetRandomStadistics(ABaseEnemy* _Enemy, EEnemyType _EnemyType, int _WaypointDataIndex);
	void CleanEnemyReferences();
};
